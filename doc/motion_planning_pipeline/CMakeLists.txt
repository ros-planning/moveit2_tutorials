add_executable(motion_planning_pipeline_tutorial
    src/motion_planning_pipeline_tutorial.cpp)
target_include_directories(motion_planning_pipeline_tutorial
    PUBLIC include)
ament_target_dependencies(motion_planning_pipeline_tutorial
    ${THIS_PACKAGE_INCLUDE_DEPENDS} Boost)

install(TARGETS motion_planning_pipeline_tutorial
    DESTINATION lib/${PROJECT_NAME}
)
install(DIRECTORY launch
    DESTINATION share/${PROJECT_NAME}
)
