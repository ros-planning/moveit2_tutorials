add_executable(mtc_tutorial src/mtc_node.cpp)
ament_target_dependencies(mtc_tutorial ${THIS_PACKAGE_INCLUDE_DEPENDS})

add_executable(minimal_mtc_tutorial src/minimal.cpp)
ament_target_dependencies(minimal_mtc_tutorial ${THIS_PACKAGE_INCLUDE_DEPENDS})

install(TARGETS mtc_tutorial minimal_mtc_tutorial
        ARCHIVE DESTINATION lib
        LIBRARY DESTINATION lib
        RUNTIME DESTINATION lib/${PROJECT_NAME})

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)
