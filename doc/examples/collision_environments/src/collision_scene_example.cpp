#include <rclcpp/rclcpp.hpp>

// MoveIt
#include <moveit/common_planning_interface_objects/common_objects.h>
#include <moveit/kinematic_constraints/utils.h>
#include <moveit/move_group_interface/move_group_interface.h>
#include <moveit/planning_scene/planning_scene.h>
#include <moveit/planning_scene_interface/planning_scene_interface.h>
#include <moveit/robot_model_loader/robot_model_loader.h>
// TF2
#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>

static const rclcpp::Logger LOGGER = rclcpp::get_logger("collision_scene_example");

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions node_options;
  node_options.automatically_declare_parameters_from_overrides(true);
  auto collision_scene_example_node = rclcpp::Node::make_shared("planning_scene_tutorial", node_options);

  moveit::planning_interface::PlanningSceneInterface planning_scene_interface_;
  // Create vector to hold 3 collision objects.
  std::vector<moveit_msgs::msg::CollisionObject> collision_objects(3);

  // Add the first table where the cube will originally be kept.
  collision_objects.at(0).id = "table1";
  collision_objects.at(0).header.frame_id = "panda_link0";

  // Create identity rotation quaternion
  tf2::Quaternion zero_orientation;
  zero_orientation.setRPY(0, 0, 0);
  const geometry_msgs::msg::Quaternion zero_orientation_msg = tf2::toMsg(zero_orientation);

  // Define the primitive and its dimensions.
  collision_objects[0].primitives.resize(1);
  collision_objects[0].primitives[0].type = collision_objects[0].primitives[0].BOX;
  collision_objects[0].primitives[0].dimensions.resize(3);
  collision_objects[0].primitives[0].dimensions[0] = 0.2;
  collision_objects[0].primitives[0].dimensions[1] = 0.4;
  collision_objects[0].primitives[0].dimensions[2] = 0.4;

  // Define the pose of the table.
  collision_objects[0].primitive_poses.resize(1);
  collision_objects[0].primitive_poses[0].position.x = 0.5;
  collision_objects[0].primitive_poses[0].position.y = 0;
  collision_objects[0].primitive_poses[0].position.z = 0.2;
  collision_objects[0].primitive_poses[0].orientation = zero_orientation_msg;

  collision_objects[0].operation = collision_objects[0].ADD;

  // Add the second table where we will be placing the cube.
  collision_objects[1].id = "table2";
  collision_objects[1].header.frame_id = "panda_link0";

  // Define the primitive and its dimensions.
  collision_objects[1].primitives.resize(1);
  collision_objects[1].primitives[0].type = collision_objects[1].primitives[0].BOX;
  collision_objects[1].primitives[0].dimensions.resize(3);
  collision_objects[1].primitives[0].dimensions[0] = 0.4;
  collision_objects[1].primitives[0].dimensions[1] = 0.2;
  collision_objects[1].primitives[0].dimensions[2] = 0.4;

  // Define the pose of the table.
  collision_objects[1].primitive_poses.resize(1);
  collision_objects[1].primitive_poses[0].position.x = 0;
  collision_objects[1].primitive_poses[0].position.y = 0.5;
  collision_objects[1].primitive_poses[0].position.z = 0.2;
  collision_objects[1].primitive_poses[0].orientation = zero_orientation_msg;

  collision_objects[1].operation = collision_objects[1].ADD;

  // Define the object that we will be manipulating.
  collision_objects[2].header.frame_id = "panda_link0";
  collision_objects[2].id = "object";

  // Define the primitive and its dimensions.
  collision_objects[2].primitives.resize(1);
  collision_objects[2].primitives[0].type = collision_objects[1].primitives[0].BOX;
  collision_objects[2].primitives[0].dimensions.resize(3);
  collision_objects[2].primitives[0].dimensions[0] = 0.02;
  collision_objects[2].primitives[0].dimensions[1] = 0.02;
  collision_objects[2].primitives[0].dimensions[2] = 0.2;

  // Define the pose of the object.
  collision_objects[2].primitive_poses.resize(1);
  collision_objects[2].primitive_poses[0].position.x = 0.5;
  collision_objects[2].primitive_poses[0].position.y = 0;
  collision_objects[2].primitive_poses[0].position.z = 0.5;
  collision_objects[2].primitive_poses[0].orientation = zero_orientation_msg;

  collision_objects[2].operation = collision_objects[2].ADD;
  planning_scene_interface_.applyCollisionObjects(collision_objects);

  rclcpp::shutdown();
  return 0;
}
