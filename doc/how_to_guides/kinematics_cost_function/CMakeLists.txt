add_executable(kinematics_cost_function_tutorial
    src/kinematics_cost_function_tutorial.cpp)
target_include_directories(kinematics_cost_function_tutorial
    PUBLIC include)
ament_target_dependencies(kinematics_cost_function_tutorial
    ${THIS_PACKAGE_INCLUDE_DEPENDS})

install(TARGETS kinematics_cost_function_tutorial
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)
