cmake_minimum_required(VERSION 3.22)
project(moveit2_tutorials)

# Common cmake code applied to all moveit packages
find_package(moveit_common REQUIRED)
moveit_package()

set(THIS_PACKAGE_INCLUDE_DIRS
  doc/interactivity/include
)

set(THIS_PACKAGE_INCLUDE_DEPENDS
  ament_cmake
  Boost
  control_msgs
  Eigen3
  interactive_markers
  moveit_core
  moveit_msgs
  moveit_ros_planning
  moveit_ros_planning_interface
  moveit_servo
  moveit_task_constructor_core
  moveit_visual_tools
  pluginlib
  rclcpp
  rclcpp_action
  rviz_visual_tools
  tf2_geometry_msgs
  tf2_geometry_msgs
  tf2_ros
)

foreach(Dependency IN ITEMS ${THIS_PACKAGE_INCLUDE_DEPENDS})
  find_package(${Dependency} REQUIRED)
endforeach()

include_directories(${THIS_PACKAGE_INCLUDE_DIRS})

# add_subdirectory(doc/examples/bullet_collision_checker)
# add_subdirectory(doc/examples/collision_environments)
# add_subdirectory(doc/examples/controller_configuration)
# add_subdirectory(doc/examples/creating_moveit_plugins/lerp_motion_planner)
# add_subdirectory(doc/examples/interactivity)
# add_subdirectory(doc/examples/kinematics)
# add_subdirectory(doc/examples/move_group_python_interface)
# add_subdirectory(doc/examples/perception_pipeline)
# add_subdirectory(doc/examples/pick_place)
# add_subdirectory(doc/examples/planning)
# add_subdirectory(doc/examples/state_display)
# add_subdirectory(doc/examples/subframes)
# add_subdirectory(doc/examples/tests)
# add_subdirectory(doc/examples/trajopt_planner)
# add_subdirectory(doc/examples/visualizing_collisions)
add_subdirectory(doc/examples/jupyter_notebook_prototyping)
add_subdirectory(doc/examples/motion_planning_api)
add_subdirectory(doc/examples/motion_planning_pipeline)
add_subdirectory(doc/examples/motion_planning_python_api)
add_subdirectory(doc/examples/move_group_interface)
add_subdirectory(doc/examples/moveit_cpp)
add_subdirectory(doc/examples/planning_scene)
add_subdirectory(doc/examples/planning_scene_ros_api)
add_subdirectory(doc/examples/realtime_servo)
add_subdirectory(doc/examples/robot_model_and_robot_state)
add_subdirectory(doc/how_to_guides/isaac_panda)
add_subdirectory(doc/how_to_guides/kinematics_cost_function)
add_subdirectory(doc/how_to_guides/parallel_planning)
add_subdirectory(doc/how_to_guides/chomp_planner)
add_subdirectory(doc/how_to_guides/persistent_scenes_and_states)
add_subdirectory(doc/how_to_guides/pilz_industrial_motion_planner)
add_subdirectory(doc/how_to_guides/using_ompl_constrained_planning)
add_subdirectory(doc/tutorials/pick_and_place_with_moveit_task_constructor)
add_subdirectory(doc/tutorials/quickstart_in_rviz)
add_subdirectory(doc/how_to_guides/pick_ik)

ament_export_dependencies(
  ${THIS_PACKAGE_INCLUDE_DEPENDS}
)

ament_export_include_directories(include)

ament_package()
