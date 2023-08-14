Examples
========

This is a catch-all place for pages that have not yet been updated for the new structure or are still not yet ported from ROS 1.

To migrate these pages into the new structure please see the appropriate pages under :doc:`/doc/how_to_contribute/how_to_contribute`.

MoveGroup - ROS Wrappers in C++
------------------------------------------
The simplest way to use MoveIt through scripting is using the ``move_group_interface``. This interface is ideal for beginners and provides unified access to many of the features of MoveIt.

.. toctree::
   :maxdepth: 1

   move_group_interface/move_group_interface_tutorial

Using MoveIt Directly Through the C++ API
-----------------------------------------
Building more complex applications with MoveIt often requires developers to dig into MoveIt’s C++ API. As an added plus, using the C++ API directly skips many of the ROS Service/Action layers resulting in significantly faster performance.

.. toctree::
   :maxdepth: 1

   robot_model_and_robot_state/robot_model_and_robot_state_tutorial
   planning_scene/planning_scene_tutorial
   planning_scene_monitor/planning_scene_monitor_tutorial
   planning_scene_ros_api/planning_scene_ros_api_tutorial
   motion_planning_api/motion_planning_api_tutorial
   motion_planning_pipeline/motion_planning_pipeline_tutorial
   creating_moveit_plugins/plugin_tutorial
   visualizing_collisions/visualizing_collisions_tutorial
   time_parameterization/time_parameterization_tutorial
   planning_with_approximated_constraint_manifolds/planning_with_approximated_constraint_manifolds_tutorial
   pick_place/pick_place_tutorial
   moveit_grasps/moveit_grasps_tutorial
   moveit_deep_grasps/moveit_deep_grasps_tutorial
   subframes/subframes_tutorial
   moveit_cpp/moveitcpp_tutorial
   bullet_collision_checker/bullet_collision_checker
   mobile_base_arm/mobile_base_arm_tutorial

Using MoveIt Directly Through the Python API
--------------------------------------------
The MoveIt Python API binds a subset of the C++ API. The Python API is useful for rapid prototyping and experimentation, or if you already are working within a Python development environment.

.. toctree::
   :maxdepth: 1

   motion_planning_python_api/motion_planning_python_api_tutorial
   jupyter_notebook_prototyping/jupyter_notebook_prototyping_tutorial

Integration with a New Robot
----------------------------
Before attempting to integrate a new robot with MoveIt 2, check whether your robot has already been set up (see the `list of robots running MoveIt <http://moveit.ros.org/robots/>`_). Otherwise, follow the tutorials in this section to integrate your robot with MoveIt.

.. toctree::
   :maxdepth: 1

   setup_assistant/setup_assistant_tutorial
   urdf_srdf/urdf_srdf_tutorial
   controller_configuration/controller_configuration_tutorial
   perception_pipeline/perception_pipeline_tutorial
   hand_eye_calibration/hand_eye_calibration_tutorial
   ikfast/ikfast_tutorial
   trac_ik/trac_ik_tutorial

Configuration
-------------
.. toctree::
   :maxdepth: 1

   kinematics_configuration/kinematics_configuration_tutorial
   custom_constraint_samplers/custom_constraint_samplers_tutorial
   ompl_interface/ompl_interface_tutorial
   trajopt_planner/trajopt_planner_tutorial
   planning_adapters/planning_adapters_tutorial

Miscellaneous
----------------------------

.. toctree::
   :maxdepth: 1

   benchmarking/benchmarking_tutorial
   dual_arms/dual_arms_tutorial
   hybrid_planning/hybrid_planning_tutorial
   realtime_servo/realtime_servo_tutorial
   tests/tests_tutorial
