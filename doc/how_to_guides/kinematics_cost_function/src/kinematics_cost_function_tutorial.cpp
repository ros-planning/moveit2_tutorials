/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2022, PickNik, Inc.
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *   * Neither the name of SRI International nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************/

/* Author: Wyatt Rees */
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit/robot_state/cartesian_interpolator.h>
#include <moveit/trajectory_processing/time_optimal_trajectory_generation.h>

#include <moveit_msgs/msg/display_robot_state.hpp>
#include <moveit_msgs/msg/display_trajectory.hpp>
#include <moveit_msgs/msg/attached_collision_object.hpp>
#include <moveit_msgs/msg/collision_object.hpp>

#include <tf2_eigen/tf2_eigen.hpp>
#include <moveit_visual_tools/moveit_visual_tools.h>

// All source files that use ROS logging should define a file-specific
// static const rclcpp::Logger named LOGGER, located at the top of the file
// and inside the namespace with the narrowest scope (if there is one)
static const rclcpp::Logger LOGGER = rclcpp::get_logger("kinematics_cost_fn_demo");

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions node_options;
  node_options.automatically_declare_parameters_from_overrides(true);
  auto node = rclcpp::Node::make_shared("kinematics_cost_fn_tutorial", node_options);

  // We spin up a SingleThreadedExecutor for the current state monitor to get information
  // about the robot's state.
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  std::thread([&executor]() { executor.spin(); }).detach();

  // BEGIN_TUTORIAL
  //
  // Setup
  // ^^^^^
  //
  // MoveIt operates on sets of joints called "planning groups" and stores them in an object called
  // the ``JointModelGroup``. Throughout MoveIt, the terms "planning group" and "joint model group"
  // are used interchangeably.
  static const std::string PLANNING_GROUP = "panda_arm";

  // The
  // :moveit_codedir:`MoveGroupInterface<moveit_ros/planning_interface/move_group_interface/include/moveit/move_group_interface/move_group_interface.h>`
  // class can be easily set up using just the name of the planning group you would like to control and plan for.
  moveit::planning_interface::MoveGroupInterface move_group(node, PLANNING_GROUP);

  // Raw pointers are frequently used to refer to the planning group for improved performance.
  const moveit::core::JointModelGroup* joint_model_group =
      move_group.getCurrentState()->getJointModelGroup(PLANNING_GROUP);

  // Visualization
  // ^^^^^^^^^^^^^
  namespace rvt = rviz_visual_tools;
  moveit_visual_tools::MoveItVisualTools visual_tools(node, "panda_link0", "move_group_tutorial",
                                                      move_group.getRobotModel());

  visual_tools.deleteAllMarkers();
  visual_tools.loadRemoteControl();

  Eigen::Isometry3d text_pose = Eigen::Isometry3d::Identity();
  text_pose.translation().z() = 1.0;
  visual_tools.publishText(text_pose, "KinematicsCostFn_Demo", rvt::WHITE, rvt::XLARGE);

  visual_tools.trigger();

  // Start the demo
  // ^^^^^^^^^^^^^^^^^^^^^^^^^
  visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to start the demo");

  // Computing IK solutions with cost functions
  // ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
  //
  // When computing IK solutions for goal poses, we can specify a cost function that will be used to
  // evaluate the "fitness" for a particular solution. At the time of writing this tutorial, this is
  // only supported by the bio_ik kinematics plugin.
  //
  //
  // To start, we'll create two pointers that references the current robot's state.
  // RobotState is the object that contains all the current position/velocity/acceleration data.
  // By making two copies, we can test the difference between IK calls that do/don't include cost functions
  moveit::core::RobotStatePtr current_state = move_group.getCurrentState(10);
  moveit::core::RobotStatePtr copy_state = move_group.getCurrentState(10);
  // We store the starting joint positions so we can evaluate performance later.
  std::vector<double> start_joint_positions;
  current_state->copyJointGroupPositions(joint_model_group, start_joint_positions);

  // Set a target pose that we would like to solve IK for
  geometry_msgs::msg::Pose target_pose;
  target_pose.orientation.w = 1.0;
  target_pose.position.x = 0.28;
  target_pose.position.y = -0.2;
  target_pose.position.z = 0.5;

  moveit::core::GroupStateValidityCallbackFn callback_fn;

  // Cost functions usually require one to accept approximate IK solutions
  kinematics::KinematicsQueryOptions opts;
  opts.return_approximate_solution = true;

  // Our cost function will optimize for minimal joint movement. Note that this is not the cost
  // function that we pass directly to the IK call, but a helper function that we can also use
  // to evaluate the solution.
  const auto compute_l2_norm = [](std::vector<double> solution, std::vector<double> start) {
    double sum = 0.0;
    for (size_t ji = 0; ji < solution.size(); ji++)
    {
      double d = solution[ji] - start[ji];
      sum += d * d;
    }
    return sum;
  };

  // The weight of the cost function often needs tuning. A tradeoff exists between the accuracy of the
  // solution pose and the extent to which the IK solver obeys our cost function.
  const double weight = 0.0005;
  const auto cost_fn = [&weight, &compute_l2_norm](const geometry_msgs::msg::Pose& /*goal_pose*/,
                                                   const moveit::core::RobotState& solution_state,
                                                   moveit::core::JointModelGroup const* jmg,
                                                   const std::vector<double>& seed_state) {
    std::vector<double> proposed_joint_positions;
    solution_state.copyJointGroupPositions(jmg, proposed_joint_positions);
    double cost = compute_l2_norm(proposed_joint_positions, seed_state);
    return weight * cost;
  };

  // Now, we request an IK solution twice for the same pose. Once with a cost function, and once without.
  current_state->setFromIK(joint_model_group, target_pose, 0.0 /* timeout */, callback_fn, opts, cost_fn);
  copy_state->setFromIK(joint_model_group, target_pose, 0.0 /* timeout */, callback_fn, opts);

  std::vector<double> solution;
  current_state->copyJointGroupPositions(joint_model_group, solution);

  std::vector<double> solution_no_cost_fn;
  copy_state->copyJointGroupPositions(joint_model_group, solution_no_cost_fn);

  // Now we can use our helper function from earlier to evaluate the effectiveness of the cost function.
  double l2_solution = compute_l2_norm(solution, start_joint_positions);
  RCLCPP_INFO_STREAM(LOGGER, "L2 norm of the solution WITH a cost function is " << l2_solution);
  l2_solution = compute_l2_norm(solution_no_cost_fn, start_joint_positions);
  RCLCPP_INFO_STREAM(LOGGER, "L2 norm of the solution WITHOUT a cost function is " << l2_solution);

  // If we're happy with the solution, we can set it as a joint value target.
  move_group.setJointValueTarget(solution);

  // We lower the allowed maximum velocity and acceleration to 5% of their maximum.
  // The default values are 10% (0.1).
  // Set your preferred defaults in the joint_limits.yaml file of your robot's moveit_config
  // or set explicit factors in your code if you need your robot to move faster.
  move_group.setMaxVelocityScalingFactor(0.05);
  move_group.setMaxAccelerationScalingFactor(0.05);

  moveit::planning_interface::MoveGroupInterface::Plan my_plan;
  bool success = (move_group.plan(my_plan) == moveit::core::MoveItErrorCode::SUCCESS);
  RCLCPP_INFO(LOGGER, "Visualizing plan 1 (joint space goal with cost function) %s", success ? "" : "FAILED");

  // Visualize the plan in RViz:
  visual_tools.deleteAllMarkers();
  visual_tools.publishAxisLabeled(target_pose, "goal_pose");
  visual_tools.publishText(text_pose, "Joint_Space_Goal", rvt::WHITE, rvt::XLARGE);
  visual_tools.publishTrajectoryLine(my_plan.trajectory, joint_model_group);
  visual_tools.trigger();

  // Uncomment if you would like to execute the motion
  /*
  visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to execute the motion");
  move_group.execute(my_plan);
  */

  visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to continue.");

  // We can also specify cost functions when computing robot trajectories that must follow a cartesian path.
  // First, let's change the target pose for the end of the path, so that if the previous motion plan was executed,
  // we still have somewhere to move.
  target_pose.position.y += 0.4;
  target_pose.position.z -= 0.1;
  target_pose.orientation.w = 0;
  target_pose.orientation.x = -1.0;
  Eigen::Isometry3d target;
  tf2::fromMsg(target_pose, target);

  auto start_state = move_group.getCurrentState(10.0);
  std::vector<moveit::core::RobotStatePtr> traj;
  moveit::core::MaxEEFStep max_eef_step(0.01, 0.1);
  // Here, we're effectively disabling the jump threshold for joints. This is not recommended on real hardware.
  moveit::core::JumpThreshold jump_thresh(0.0);

  // The trajectory, traj, passed to computeCartesianPath will contain several waypoints toward
  // the goal pose, target. For each of these waypoints, the IK solver is queried with the given cost function.
  const auto frac = moveit::core::CartesianInterpolator::computeCartesianPath(
      start_state.get(), joint_model_group, traj, joint_model_group->getLinkModel("panda_link8"), target, true,
      max_eef_step, jump_thresh, callback_fn, opts, cost_fn);

  RCLCPP_INFO(LOGGER, "Computed %f percent of cartesian path.", frac.value * 100.0);

  // Once we've computed the points in our Cartesian path, we need to add timestamps to each point for execution.
  robot_trajectory::RobotTrajectory rt(start_state->getRobotModel(), PLANNING_GROUP);
  for (const moveit::core::RobotStatePtr& traj_state : traj)
    rt.addSuffixWayPoint(traj_state, 0.0);
  trajectory_processing::TimeOptimalTrajectoryGeneration time_param;
  time_param.computeTimeStamps(rt, 1.0);

  moveit_msgs::msg::RobotTrajectory rt_msg;
  rt.getRobotTrajectoryMsg(rt_msg);

  visual_tools.deleteAllMarkers();
  visual_tools.publishTrajectoryLine(rt, joint_model_group);
  visual_tools.publishText(text_pose, "Cartesian_Path_Goal", rvt::WHITE, rvt::XLARGE);
  visual_tools.publishAxisLabeled(target_pose, "cartesian_goal_pose");
  visual_tools.trigger();

  // Once we've computed the timestamps, we can pass the trajectory to move_group to execute it.
  move_group.execute(rt_msg);
  visual_tools.prompt("Press 'next' in the RvizVisualToolsGui window to end the demo.");

  // END_TUTORIAL
  visual_tools.deleteAllMarkers();
  visual_tools.trigger();

  rclcpp::shutdown();
  return 0;
}
