#include <memory>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit_visual_tools/moveit_visual_tools.h>

/**
 * Pilz Example -- MoveGroup Interface
 *
 * To run this example, first run this launch file:
 * ros2 launch moveit2_tutorials demo.launch.py rviz_config:=panda_hello_moveit.rviz
 *
 * For best results, hide the "MotionPlanning" widget in RViz.
 *
 * Then, run this file:
 * ros2 run moveit2_tutorials pilz_move_group
 */

int main(int argc, char* argv[])
{
  // Initialize ROS and create the Node
  rclcpp::init(argc, argv);
  auto const node = std::make_shared<rclcpp::Node>(
      "pilz_move_group_node", rclcpp::NodeOptions().automatically_declare_parameters_from_overrides(true));

  // We spin up a SingleThreadedExecutor so MoveItVisualTools interact with ROS
  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(node);
  auto spinner = std::thread([&executor]() { executor.spin(); });

  // Create a ROS logger
  auto const logger = rclcpp::get_logger("pilz_move_group");

  // Create the MoveIt MoveGroup Interface
  using moveit::planning_interface::MoveGroupInterface;
  auto move_group_interface = MoveGroupInterface(node, "panda_arm");

  // Construct and initialize MoveItVisualTools
  auto moveit_visual_tools =
      moveit_visual_tools::MoveItVisualTools{ node, "panda_link0", rviz_visual_tools::RVIZ_MARKER_TOPIC,
                                              move_group_interface.getRobotModel() };
  moveit_visual_tools.deleteAllMarkers();
  moveit_visual_tools.loadRemoteControl();

  // Create closures for visualization
  auto const draw_title = [&moveit_visual_tools](const auto& text) {
    auto const text_pose = [] {
      auto msg = Eigen::Isometry3d::Identity();
      msg.translation().z() = 1.0;
      return msg;
    }();
    moveit_visual_tools.publishText(text_pose, text, rviz_visual_tools::WHITE, rviz_visual_tools::XLARGE);
  };
  auto const prompt = [&moveit_visual_tools](const auto& text) { moveit_visual_tools.prompt(text); };
  auto const draw_trajectory_tool_path =
      [&moveit_visual_tools, jmg = move_group_interface.getRobotModel()->getJointModelGroup("panda_arm")](
          auto const trajectory) { moveit_visual_tools.publishTrajectoryLine(trajectory, jmg); };

  // Helper to plan and execute motion
  auto const plan_and_execute = [&](const std::string& title) {
    prompt("Press 'Next' in the RVizVisualToolsGui window to plan");
    draw_title("Planning " + title);
    moveit_visual_tools.trigger();
    auto const [success, plan] = [&move_group_interface] {
      moveit::planning_interface::MoveGroupInterface::Plan msg;
      auto const ok = static_cast<bool>(move_group_interface.plan(msg));
      return std::make_pair(ok, msg);
    }();

    // Execute the plan
    if (success)
    {
      draw_trajectory_tool_path(plan.trajectory);
      moveit_visual_tools.trigger();
      prompt("Press 'Next' in the RVizVisualToolsGui window to execute");
      draw_title("Executing " + title);
      moveit_visual_tools.trigger();
      move_group_interface.execute(plan);
    }
    else
    {
      RCLCPP_ERROR(logger, "Planning failed!");
      draw_title("Planning Failed!");
      moveit_visual_tools.trigger();
    }
  };

  // Plan and execute a multi-step sequence using Pilz
  move_group_interface.setPlanningPipelineId("pilz_industrial_motion_planner");

  {
    // Move to a pre-grasp pose
    move_group_interface.setPlannerId("PTP");
    auto const pre_grasp_pose = [] {
      geometry_msgs::msg::PoseStamped msg;
      msg.header.frame_id = "world";
      msg.pose.orientation.x = 1.0;
      msg.pose.orientation.y = 0.0;
      msg.pose.orientation.z = 0.0;
      msg.pose.orientation.w = 0.0;
      msg.pose.position.x = 0.6;
      msg.pose.position.y = -0.2;
      msg.pose.position.z = 0.6;
      return msg;
    }();
    move_group_interface.setPoseTarget(pre_grasp_pose, "panda_hand");
    plan_and_execute("[PTP] Approach");
  }

  {
    // Move in a linear trajectory to a grasp pose using the LIN planner.
    move_group_interface.setPlannerId("LIN");
    auto const grasp_pose = [] {
      geometry_msgs::msg::PoseStamped msg;
      msg.header.frame_id = "world";
      msg.pose.orientation.x = 1.0;
      msg.pose.orientation.y = 0.0;
      msg.pose.orientation.z = 0.0;
      msg.pose.orientation.w = 0.0;
      msg.pose.position.x = 0.6;
      msg.pose.position.y = -0.2;
      msg.pose.position.z = 0.4;
      return msg;
    }();
    move_group_interface.setPoseTarget(grasp_pose, "panda_hand");
    plan_and_execute("[LIN] Grasp");
  }

  {
    // Move in a circular arc motion using the CIRC planner.
    move_group_interface.setPlannerId("CIRC");
    auto const goal_pose = [] {
      geometry_msgs::msg::PoseStamped msg;
      msg.header.frame_id = "world";
      msg.pose.orientation.x = 0.7071;
      msg.pose.orientation.y = 0.0;
      msg.pose.orientation.z = 0.0;
      msg.pose.orientation.w = 0.7071;
      msg.pose.position.x = 0.6;
      msg.pose.position.y = 0.0;
      msg.pose.position.z = 0.6;
      return msg;
    }();
    move_group_interface.setPoseTarget(goal_pose, "panda_hand");

    // Set a constraint pose. This is the center of the arc.
    auto const center_pose = [] {
      geometry_msgs::msg::PoseStamped msg;
      msg.header.frame_id = "world";
      msg.pose.orientation.x = 1.0;
      msg.pose.orientation.y = 0.0;
      msg.pose.orientation.z = 0.0;
      msg.pose.orientation.w = 0.0;
      msg.pose.position.x = 0.6;
      msg.pose.position.y = 0.0;
      msg.pose.position.z = 0.4;
      return msg;
    }();
    moveit_msgs::msg::Constraints constraints;
    constraints.name = "center";  // Change to "interim" to use an intermediate point on arc instead.
    moveit_msgs::msg::PositionConstraint pos_constraint;
    pos_constraint.header.frame_id = center_pose.header.frame_id;
    pos_constraint.link_name = "panda_hand";
    pos_constraint.constraint_region.primitive_poses.push_back(center_pose.pose);
    pos_constraint.weight = 1.0;
    constraints.position_constraints.push_back(pos_constraint);
    move_group_interface.setPathConstraints(constraints);

    plan_and_execute("[CIRC] Turn");
  }

  {
    // Move back home using the PTP planner.
    move_group_interface.setPlannerId("PTP");
    move_group_interface.setNamedTarget("ready");
    plan_and_execute("[PTP] Return");
  }

  // Shutdown ROS
  spinner.join();
  rclcpp::shutdown();
  return 0;
}
