/*********************************************************************
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2023, PickNik Inc.
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
 *   * Neither the name of PickNik Inc. nor the names of its
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

/*      Title       : pose_tracking_tutorial.cpp
 *      Project     : moveit2_tutorials
 *      Created     : 08/07/2023
 *      Author      : V Mohammed Ibrahim
 *
 *      Description : Example of using pose tracking via the ROS API in a door opening scenario.
 */

#include <rclcpp/rclcpp.hpp>
#include <tf2_eigen/tf2_eigen.hpp>
#include <moveit_msgs/msg/planning_scene.hpp>
#include <visualization_msgs/msg/marker.hpp>
#include <visualization_msgs/msg/marker_array.hpp>
#include <geometry_msgs/msg/pose_stamped.hpp>
#include <moveit_servo/utils/common.hpp>
#include <moveit_msgs/srv/servo_command_type.hpp>

/**
 * \brief Handles the simulation of the collision object representing the door.
 */
class Door
{
public:
  Door(const rclcpp::Node::SharedPtr& node) : node_(node)
  {
    dims_ = Eigen::Vector3d(0.5, 0.02, 0.8);
    rotation_radius_ = dims_.x() / 2;
    // Hinge is the bottom left corner
    hinge_ = Eigen::Vector3d(0.8, 0.0, 0.0);

    center_.x() = hinge_.x() + dims_.x() / 2;
    center_.y() = hinge_.y() + dims_.y() / 2 + rotation_radius_;
    center_.z() = hinge_.z() + dims_.z() / 2;

    angle_ = (M_PI / 2);

    collision_object_publisher_ =
        node_->create_publisher<moveit_msgs::msg::PlanningScene>("/planning_scene", rclcpp::SystemDefaultsQoS());

    createCollisionObject();
  }

  void rotateDoor(double angle)
  {
    angle_ = angle;

    collision_object_.primitives[0] = door_primitive_;

    center_.x() = hinge_.x() + (rotation_radius_ * cos(angle_));
    center_.y() = hinge_.y() + (rotation_radius_ * sin(angle_));
    geometry_msgs::msg::Pose door_pose;
    door_pose.position.x = center_.x();
    door_pose.position.y = center_.y();
    door_pose.position.z = center_.z();
    auto orn = Eigen::Quaterniond(Eigen::AngleAxisd(angle_, Eigen::Vector3d::UnitZ()));
    door_pose.orientation.w = orn.w();
    door_pose.orientation.x = orn.x();
    door_pose.orientation.y = orn.y();
    door_pose.orientation.z = orn.z();

    collision_object_.operation = collision_object_.ADD;
    collision_object_.primitive_poses[0] = door_pose;
    collision_object_.header.stamp = node_->now();

    moveit_msgs::msg::PlanningSceneWorld psw;
    psw.collision_objects.push_back(collision_object_);

    moveit_msgs::msg::PlanningScene ps;
    ps.world = psw;
    ps.is_diff = true;
    collision_object_publisher_->publish(ps);
  }

private:
  void createCollisionObject()
  {
    collision_object_.id = "door";
    collision_object_.header.frame_id = "panda_link0";
    collision_object_.primitives.resize(1);
    collision_object_.primitive_poses.resize(1);

    door_primitive_.type = shape_msgs::msg::SolidPrimitive::BOX;
    door_primitive_.dimensions = { dims_[0], dims_[1], dims_[2] };
  }

  rclcpp::Node::SharedPtr node_;
  Eigen::Vector3d hinge_, center_, dims_;
  double angle_, step_, rotation_radius_;
  rclcpp::Publisher<moveit_msgs::msg::PlanningScene>::SharedPtr collision_object_publisher_;
  moveit_msgs::msg::CollisionObject collision_object_;
  shape_msgs::msg::SolidPrimitive door_primitive_;
};

/**
 * \brief Generates the path to follow when opening the door.
 */
std::vector<Eigen::Vector3d> getPath()
{
  const double start_angle = M_PI / 2 + (M_PI / 8);
  const double end_angle = M_PI;
  const double step = 0.01745329;
  std::vector<Eigen::Vector3d> traj;

  for (double i = start_angle; i < end_angle; i = i + step)
  {
    double x = 0.8 + (0.5 * cos(i));
    double y = 0.0 + (0.5 * sin(i));
    auto vec = Eigen::Vector3d(x, y, 0.4);
    traj.push_back(vec);
  }
  return traj;
}

/**
 * \brief Creates an Rviz marker message to represent a waypoint in the path.
 */
visualization_msgs::msg::Marker getMarker(int id, const Eigen::Vector3d& position, const std::string& frame)
{
  visualization_msgs::msg::Marker marker;
  marker.header.frame_id = frame;
  marker.header.stamp = rclcpp::Time(0.0);
  marker.id = id;
  marker.type = visualization_msgs::msg::Marker::SPHERE;
  marker.action = visualization_msgs::msg::Marker::ADD;
  marker.pose.position.x = position.x();
  marker.pose.position.y = position.y();
  marker.pose.position.z = position.z();
  marker.pose.orientation.x = 0.0;
  marker.pose.orientation.y = 0.0;
  marker.pose.orientation.z = 0.0;
  marker.pose.orientation.w = 1.0;
  marker.scale.x = 0.01;
  marker.scale.y = 0.01;
  marker.scale.z = 0.01;
  marker.color.a = 1.0;
  marker.color.r = 0.0;
  marker.color.g = 1.0;
  marker.color.b = 0.0;
  id++;
  return marker;
}

/**
 * \brief Generates a PoseStamped message with the given position and orientation.
 */
geometry_msgs::msg::PoseStamped getPose(const Eigen::Vector3d& position, const Eigen::Quaterniond& rotation)
{
  geometry_msgs::msg::PoseStamped target_pose;
  target_pose.header.frame_id = "panda_link0";
  target_pose.pose.orientation.w = rotation.w();
  target_pose.pose.orientation.x = rotation.x();
  target_pose.pose.orientation.y = rotation.y();
  target_pose.pose.orientation.z = rotation.z();
  target_pose.pose.position.x = position.x();
  target_pose.pose.position.y = position.y();
  target_pose.pose.position.z = position.z();

  return target_pose;
}

int main(int argc, char* argv[])
{
  rclcpp::init(argc, argv);
  rclcpp::Node::SharedPtr node = std::make_shared<rclcpp::Node>("servo_tutorial");

  auto marker_publisher =
      node->create_publisher<visualization_msgs::msg::MarkerArray>("/path_markers", rclcpp::SystemDefaultsQoS());
  auto pose_publisher = node->create_publisher<geometry_msgs::msg::PoseStamped>("/servo_node/pose_target_cmds",
                                                                                rclcpp::SystemDefaultsQoS());

  auto switch_input_client = node->create_client<moveit_msgs::srv::ServoCommandType>("/servo_node/switch_command_type");

  auto executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
  executor->add_node(node);

  // Spin the node.
  std::thread executor_thread([&]() { executor->spin(); });

  visualization_msgs::msg::MarkerArray marray;
  std::vector<Eigen::Vector3d> path = getPath();

  for (size_t i = 0; i < path.size(); i++)
  {
    marray.markers.push_back(getMarker(i, path[i], "panda_link0"));
  }
  marker_publisher->publish(marray);

  // Create the door.
  Door door(node);
  door.rotateDoor(M_PI / 2);

  // Default end-effector pose of the Panda arm.
  auto ee_pose = Eigen::Isometry3d::Identity();
  ee_pose.rotate(Eigen::AngleAxisd(M_PI, Eigen::Vector3d::UnitX()));
  ee_pose.translate(Eigen::Vector3d(0.3, 0.0, 0.4));

  // Create the ROS message.
  auto request = std::make_shared<moveit_msgs::srv::ServoCommandType::Request>();
  request->command_type = moveit_msgs::srv::ServoCommandType::Request::POSE;
  auto response = switch_input_client->async_send_request(request);
  if (response.get()->success)
  {
    RCLCPP_INFO_STREAM(node->get_logger(), "Switched to input type: Pose");
  }
  else
  {
    RCLCPP_WARN_STREAM(node->get_logger(), "Could not switch input to: Pose");
  }

  // Follow the trajectory

  const double publish_period = 0.15;
  rclcpp::WallRate rate(1 / publish_period);

  // The path needs to be reversed since the last point in the path is where we want to start.
  std::reverse(path.begin(), path.end());

  for (auto& waypoint : path)
  {
    auto target_pose = getPose(waypoint, Eigen::Quaterniond(ee_pose.rotation()));
    target_pose.header.stamp = node->now();
    pose_publisher->publish(target_pose);
    rate.sleep();
  }

  // Simulated door opening

  double door_angle = M_PI / 2;
  const double step = 0.01745329;  // 1 degree in radian

  // Reverse again to so that we follow that path in reverse order.
  std::reverse(path.begin(), path.end());
  for (auto& waypoint : path)
  {
    ee_pose.rotate(Eigen::AngleAxisd(-step, Eigen::Vector3d::UnitZ()));
    auto target_pose = getPose(waypoint, Eigen::Quaterniond(ee_pose.rotation()));
    target_pose.header.stamp = node->now();
    pose_publisher->publish(target_pose);
    rate.sleep();

    door.rotateDoor(door_angle);
    door_angle += step;
  }

  executor->cancel();
  if (executor_thread.joinable())
  {
    executor_thread.join();
  }
  rclcpp::shutdown();
}
