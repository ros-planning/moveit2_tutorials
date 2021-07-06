cmake_minimum_required(VERSION 3.10.2)
project(moveit2_tutorials)

# Common cmake code applied to all moveit packages
find_package(moveit_common REQUIRED)
moveit_package()

find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED system filesystem date_time thread)
find_package(ament_cmake REQUIRED)
find_package(control_msgs REQUIRED)
find_package(moveit_core REQUIRED)
find_package(moveit_ros_planning REQUIRED)
find_package(moveit_ros_planning_interface REQUIRED)
find_package(moveit_ros_perception REQUIRED)
find_package(moveit_servo REQUIRED)
find_package(interactive_markers REQUIRED)
find_package(rviz_visual_tools REQUIRED)
#find_package(moveit_visual_tools REQUIRED)
find_package(geometric_shapes REQUIRED)
#find_package(pcl_ros REQUIRED)
#find_package(pcl_conversions REQUIRED)
#find_package(rosbag REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_action REQUIRED)
find_package(pluginlib REQUIRED)
find_package(tf2_ros REQUIRED)
find_package(tf2_eigen REQUIRED)
find_package(tf2_geometry_msgs REQUIRED)

set(THIS_PACKAGE_INCLUDE_DIRS
  doc/interactivity/include
)

set(THIS_PACKAGE_INCLUDE_DEPENDS
  ament_cmake
  rclcpp
  rclcpp_action
  tf2_geometry_msgs
  tf2_ros
  moveit_core
  rviz_visual_tools
  # moveit_visual_tools
  moveit_ros_planning_interface
  interactive_markers
  tf2_geometry_msgs
  moveit_ros_planning
  pluginlib
  Eigen3
  Boost
  control_msgs
  moveit_servo
)

include_directories(${THIS_PACKAGE_INCLUDE_DIRS})

# add_subdirectory(doc/controller_configuration)
# add_subdirectory(doc/interactivity)
# add_subdirectory(doc/kinematics)
# add_subdirectory(doc/motion_planning_api)
# add_subdirectory(doc/motion_planning_pipeline)
add_subdirectory(doc/move_group_interface)
# add_subdirectory(doc/move_group_python_interface)
# add_subdirectory(doc/perception_pipeline)
# add_subdirectory(doc/pick_place)
# add_subdirectory(doc/planning)
add_subdirectory(doc/planning_scene)
add_subdirectory(doc/planning_scene_ros_api)
add_subdirectory(doc/quickstart_in_rviz)
# add_subdirectory(doc/robot_model_and_robot_state)
# add_subdirectory(doc/state_display)
add_subdirectory(doc/subframes)
# add_subdirectory(doc/tests)
# add_subdirectory(doc/trajopt_planner)
# add_subdirectory(doc/creating_moveit_plugins/lerp_motion_planner)
add_subdirectory(doc/moveit_cpp)
# add_subdirectory(doc/collision_environments)
# add_subdirectory(doc/visualizing_collisions)
# add_subdirectory(doc/bullet_collision_checker)
add_subdirectory(doc/realtime_servo)

ament_export_dependencies(
  ${THIS_PACKAGE_INCLUDE_DEPENDS}
)

ament_export_include_directories(include)

ament_package()
