add_executable(pose_tracking_tutorial src/pose_tracking_tutorial.cpp)
target_include_directories(pose_tracking_tutorial PUBLIC include)
ament_target_dependencies(pose_tracking_tutorial  ${THIS_PACKAGE_INCLUDE_DEPENDS})

install(
  TARGETS
    pose_tracking_tutorial
  DESTINATION
    lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)

install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)
