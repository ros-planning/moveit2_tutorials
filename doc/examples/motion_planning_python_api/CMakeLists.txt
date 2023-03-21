install(PROGRAMS
  scripts/motion_planning_python_api_tutorial.py
  scripts/motion_planning_python_api_planning_scene.py
  DESTINATION lib/${PROJECT_NAME}
)

install(DIRECTORY config launch
	DESTINATION share/${PROJECT_NAME}
	)
