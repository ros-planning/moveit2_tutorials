find_package(PCL REQUIRED)
set(SUBTUTORIAL_DEPENDS
  geometry_msgs
  pcl_conversions
  pcl_ros
  sensor_msgs
)
foreach(dep IN ITEMS ${SUBTUTORIAL_DEPENDS})
  find_package(${dep} REQUIRED)
endforeach()

include_directories(include ${PCL_INCLUDE_DIRS})

set(APP_CYLINDER detect_and_add_cylinder_collision_object_demo)
add_executable(${APP_CYLINDER} src/detect_and_add_cylinder_collision_object_demo.cpp)
ament_target_dependencies(${APP_CYLINDER}
  ${SUBTUTORIAL_DEPENDS}
  ${THIS_PACKAGE_INCLUDE_DEPENDS})
install(TARGETS ${APP_CYLINDER} RUNTIME DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY bags launch
    DESTINATION share/${PROJECT_NAME}
)
