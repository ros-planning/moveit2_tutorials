add_executable(pilz_move_group src/pilz_move_group.cpp)
target_include_directories(pilz_move_group PUBLIC include)
ament_target_dependencies(pilz_move_group ${THIS_PACKAGE_INCLUDE_DEPENDS})

add_executable(pilz_mtc src/pilz_mtc.cpp)
target_include_directories(pilz_mtc PUBLIC include)
ament_target_dependencies(pilz_mtc ${THIS_PACKAGE_INCLUDE_DEPENDS})

install(
  TARGETS
    pilz_move_group
    pilz_mtc
  DESTINATION
    lib/${PROJECT_NAME}
)

install(DIRECTORY launch
  DESTINATION share/${PROJECT_NAME}
)
