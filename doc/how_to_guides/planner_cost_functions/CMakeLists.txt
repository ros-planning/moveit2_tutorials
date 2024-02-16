add_executable(planner_cost_functions_example src/planner_cost_functions_main.cpp)
target_include_directories(planner_cost_functions_example PRIVATE include)
ament_target_dependencies(planner_cost_functions_example  ${THIS_PACKAGE_INCLUDE_DEPENDS} Boost)

install(TARGETS planner_cost_functions_example
  DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)
install(DIRECTORY config
  DESTINATION share/${PROJECT_NAME}
)
