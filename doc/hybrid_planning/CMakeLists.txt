set(INCLUDE_DEPENDS
  rclcpp
  rclcpp_action
  moveit_core
  moveit_ros_planning
  moveit_msgs
)

add_executable(hybrid_planning_test_node src/hybrid_planning_test_node.cpp)
ament_target_dependencies(hybrid_planning_test_node ${INCLUDE_DEPENDS})

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)
