#include <rclcpp/rclcpp.hpp>
#include <moveit/task_constructor/task.h>
#include <moveit/task_constructor/solvers.h>
#include <moveit/task_constructor/stages.h>

#if __has_include(<tf2_geometry_msgs/tf2_geometry_msgs.hpp>)
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>
#else
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#endif

/**
 * Pilz Example -- MoveIt Task Constructor
 *
 * To run this example, first run this launch file:
 * ros2 launch moveit2_tutorials mtc_demo.launch.py
 *
 * Then, run this file through its own launch file:
 *
 */

static const rclcpp::Logger LOGGER = rclcpp::get_logger("pilz_mtc");
namespace mtc = moveit::task_constructor;

class MTCTaskNode
{
public:
  MTCTaskNode(const rclcpp::NodeOptions& options);

  rclcpp::node_interfaces::NodeBaseInterface::SharedPtr getNodeBaseInterface();

  void doTask();

  void setupPlanningScene();

private:
  // Compose an MTC task from a series of stages.
  mtc::Task createTask();
  mtc::Task task_;
  rclcpp::Node::SharedPtr node_;
};

rclcpp::node_interfaces::NodeBaseInterface::SharedPtr MTCTaskNode::getNodeBaseInterface()
{
  return node_->get_node_base_interface();
}

MTCTaskNode::MTCTaskNode(const rclcpp::NodeOptions& options)
  : node_{ std::make_shared<rclcpp::Node>("pilz_mtc_node", options) }
{
}

void MTCTaskNode::doTask()
{
  task_ = createTask();

  try
  {
    task_.init();
  }
  catch (mtc::InitStageException& e)
  {
    RCLCPP_ERROR_STREAM(LOGGER, e);
    return;
  }

  if (!task_.plan())
  {
    RCLCPP_ERROR_STREAM(LOGGER, "Task planning failed");
    return;
  }
  task_.introspection().publishSolution(*task_.solutions().front());

  auto result = task_.execute(*task_.solutions().front());
  if (result.val != moveit_msgs::msg::MoveItErrorCodes::SUCCESS)
  {
    RCLCPP_ERROR_STREAM(LOGGER, "Task execution failed");
    return;
  }

  return;
}

mtc::Task MTCTaskNode::createTask()
{
  mtc::Task task;
  task.stages()->setName("pilz example task");
  task.loadRobotModel(node_);

  const auto& arm_group_name = "panda_arm";
  const auto& hand_group_name = "hand";
  const auto& hand_frame = "panda_hand";

  // Set task properties
  task.setProperty("group", arm_group_name);
  task.setProperty("eef", hand_group_name);
  task.setProperty("ik_frame", hand_frame);

  // Set up a simple joint interpolation planner for the gripper
  auto interpolation_planner = std::make_shared<mtc::solvers::JointInterpolationPlanner>();

  // Set up 3 separate pilz planners with different IDs
  auto pilz_ptp_planner = std::make_shared<mtc::solvers::PipelinePlanner>(node_, "pilz_industrial_motion_planner");
  pilz_ptp_planner->setPlannerId("PTP");
  pilz_ptp_planner->setProperty("max_velocity_scaling_factor", 0.5);
  pilz_ptp_planner->setProperty("max_acceleration_scaling_factor", 0.5);

  auto pilz_lin_planner = std::make_shared<mtc::solvers::PipelinePlanner>(node_, "pilz_industrial_motion_planner");
  pilz_lin_planner->setPlannerId("LIN");
  pilz_lin_planner->setProperty("max_velocity_scaling_factor", 0.2);
  pilz_lin_planner->setProperty("max_acceleration_scaling_factor", 0.2);

  auto pilz_circ_planner = std::make_shared<mtc::solvers::PipelinePlanner>(node_, "pilz_industrial_motion_planner");
  pilz_circ_planner->setPlannerId("CIRC");
  pilz_circ_planner->setProperty("max_velocity_scaling_factor", 0.3);
  pilz_circ_planner->setProperty("max_acceleration_scaling_factor", 0.3);

  {
    // Start at current state.
    auto stage_state_current = std::make_unique<mtc::stages::CurrentState>("current");
    task.add(std::move(stage_state_current));
  }

  {
    // Go to a pre-grasp pose using the PTP planner.
    auto stage = std::make_unique<mtc::stages::MoveTo>("go to approach", pilz_ptp_planner);
    stage->setGroup(arm_group_name);
    stage->setIKFrame(hand_frame);

    // Set the approach pose.
    auto const goal_pose = [] {
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
    stage->setGoal(goal_pose);

    task.add(std::move(stage));
  }

  {
    // Move in a straight line towards the grasp using the LIN planner.
    auto stage = std::make_unique<mtc::stages::MoveTo>("go to grasp", pilz_lin_planner);
    stage->setGroup(arm_group_name);
    stage->setIKFrame(hand_frame);

    // Set the acceleration and velocity scaling factors.
    pilz_lin_planner->setProperty("max_velocity_scaling_factor", 0.3);
    pilz_lin_planner->setProperty("max_acceleration_scaling_factor", 0.3);

    // Set the post-approach pose.
    auto const goal_pose = [] {
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
    stage->setGoal(goal_pose);

    task.add(std::move(stage));
  }

  {
    // Close the hand.
    auto stage = std::make_unique<mtc::stages::MoveTo>("close hand", interpolation_planner);
    stage->setGroup(hand_group_name);
    stage->setGoal("close");
    task.add(std::move(stage));
  }

  {
    // Move in a circular arc using the CIRC planner.
    auto stage = std::make_unique<mtc::stages::MoveTo>("move circle", pilz_circ_planner);
    stage->setGroup(arm_group_name);
    stage->setIKFrame(hand_frame);

    // Set the pose goal.
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
    stage->setGoal(goal_pose);

    // Define the arc center pose.
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

    // Define the arc constraint.
    moveit_msgs::msg::PositionConstraint pos_constraint;
    pos_constraint.header.frame_id = center_pose.header.frame_id;
    pos_constraint.link_name = hand_frame;
    pos_constraint.constraint_region.primitive_poses.resize(1);
    pos_constraint.constraint_region.primitive_poses[0] = center_pose.pose;
    pos_constraint.weight = 1.0;

    moveit_msgs::msg::Constraints path_constraints;
    path_constraints.name = "center";  // Change to "interim" to use an intermediate point on arc instead.
    path_constraints.position_constraints.resize(1);
    path_constraints.position_constraints[0] = pos_constraint;
    stage->setPathConstraints(path_constraints);

    task.add(std::move(stage));
  }

  {
    // Open the hand.
    auto stage = std::make_unique<mtc::stages::MoveTo>("open hand", interpolation_planner);
    stage->setGroup(hand_group_name);
    stage->setGoal("open");
    task.add(std::move(stage));
  }

  {
    // Move in a straight line away from the grasp using the LIN planner.
    auto stage = std::make_unique<mtc::stages::MoveTo>("retract grasp", pilz_lin_planner);
    stage->setGroup(arm_group_name);
    stage->setIKFrame(hand_frame);

    // Set the retract ppose
    auto const goal_pose = [] {
      geometry_msgs::msg::PoseStamped msg;
      msg.header.frame_id = "world";
      msg.pose.orientation.x = 0.7071;
      msg.pose.orientation.y = 0.0;
      msg.pose.orientation.z = 0.0;
      msg.pose.orientation.w = 0.7071;
      msg.pose.position.x = 0.6;
      msg.pose.position.y = 0.1;
      msg.pose.position.z = 0.6;
      return msg;
    }();
    stage->setGoal(goal_pose);

    task.add(std::move(stage));
  }

  {
    // Return to the original ready pose using the PTP planner.
    auto stage = std::make_unique<mtc::stages::MoveTo>("return home", pilz_ptp_planner);
    stage->setGroup(arm_group_name);
    stage->setGoal("ready");
    task.add(std::move(stage));
  }

  return task;
}

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);

  rclcpp::NodeOptions options;
  options.automatically_declare_parameters_from_overrides(true);

  auto mtc_task_node = std::make_shared<MTCTaskNode>(options);
  rclcpp::executors::MultiThreadedExecutor executor;

  auto spin_thread = std::make_unique<std::thread>([&executor, &mtc_task_node]() {
    executor.add_node(mtc_task_node->getNodeBaseInterface());
    executor.spin();
    executor.remove_node(mtc_task_node->getNodeBaseInterface());
  });

  mtc_task_node->doTask();

  spin_thread->join();
  rclcpp::shutdown();
  return 0;
}
