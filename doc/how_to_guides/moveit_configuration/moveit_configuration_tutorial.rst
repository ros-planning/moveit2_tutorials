MoveIt Configuration
==================================

The recommended way of configuring MoveIt for your robot is by creating a colcon package containing the MoveIt Configuration.

Suppose you would like to create a configuration package for some robot named ``my_robot``.
To do this, you can create a colcon package named ``my_robot_moveit_config``, whose structure is as follows:

.. code-block::

    my_robot_moveit_config
        config/
            kinematics.yaml
            joint_limits.yaml
            *_planning.yaml
            moveit_controllers.yaml
            moveit_cpp.yaml
            sensors_3d.yaml
            ...
        launch/
        .setup_assistant
        CMakeLists.txt
        package.xml

You can create such a package manually, or use the :doc:`MoveIt Setup Assistant </doc/examples/setup_assistant/setup_assistant_tutorial>` to automatically generate it given a robot description file (URDF or xacro).

Refer to the `moveit_resources <https://github.com/ros-planning/moveit_resources/tree/ros2>`_ repository for working examples of MoveIt configuration packages.


Configuration Files Overview
----------------------------

The ``config/`` folder of a MoveIt configuration package contains several files that describe parameters for various capabilities of MoveIt.

Note that several of these files are optional depending on the functionality required at runtime.

Robot Description
^^^^^^^^^^^^^^^^^

This is the most important piece of information in a MoveIt configuration package.
There must be a URDF and SRDF file present in this folder to describe the robot kinematics, planning groups, collision rules, and more.
To learn more about these files, refer to the :doc:`URDF/SRDF Overview </doc/examples/urdf_srdf/urdf_srdf_tutorial>`.

Joint Limits
^^^^^^^^^^^^

The URDF file specification allows setting joint position and velocity limits.
However, you may want to define different joint limits for motion planning with MoveIt without modifying the underlying robot description file.
Furthermore, some features in MoveIt use additional types of joint limits, such as acceleration and jerk limits, which cannot be specified in URDF.

The default location of this file is in ``config/joint_limits.yaml``.

Here is an example snippet of a joint limits file for a simple robot with two joints:

.. code-block:: yaml

    joint_limits:
        joint1:
            has_velocity_limits: true
            max_velocity: 2.0
            has_acceleration_limits: true
            max_acceleration: 4.0
            has_jerk_limits: false
        panda_joint2:
            has_velocity_limits: true
            max_velocity: 1.5
            has_acceleration_limits: true
            max_acceleration: 3.0
            has_jerk_limits: false

Inverse Kinematics (IK) Solver
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Many motion planning applications in MoveIt require solving inverse kinematics.

Both the IK solver plugin used and its parameters are configured through a file whose default location is ``config/kinematics.yaml``.

For more information, refer to :doc:`Kinematics Configuration </doc/examples/kinematics_configuration/kinematics_configuration_tutorial>`.

Motion Planning Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For each type of motion planner plugin available in MoveIt, there is a corresponding ``config/*_planning.yaml`` file that describes its configuration.
For example, a robot that can use both :doc:`OMPL </doc/examples/ompl_interface/ompl_interface_tutorial>` and :doc:`Pilz Industrial Motion Planner </doc/how_to_guides/pilz_industrial_motion_planner/pilz_industrial_motion_planner>` will have the following folder structure:

.. code-block::

    my_robot_moveit_config
        config/
            ompl_planning.yaml
            pilz_industrial_motion_planner_planning.yaml
            ...
        ...

By default, all parameter files that match this ``config/*_planning.yaml`` pattern will be loaded.
If OMPL is configured as a planning pipeline, that will be the default; otherwise, it will be the first pipeline in the list.

To learn more about the contents of the individual planning configuration files, refer to the configuration documentation for those planners.

Trajectory Execution Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

MoveIt typically publishes manipulator motion commands to a `JointTrajectoryController <https://github.com/ros-controls/ros2_controllers/tree/master/joint_trajectory_controller>`_.
To learn more, refer to the :doc:`Low Level Controllers </doc/examples/controller_configuration/controller_configuration_tutorial>` section.

The default location for trajectory execution information is in ``config/moveit_controllers.yaml``.

MoveItCpp Configuration
^^^^^^^^^^^^^^^^^^^^^^^

If you are using :doc:`MoveItCpp </doc/examples/moveit_cpp/moveitcpp_tutorial>`, you can define a file with all the necessary parameters.

The default location of this file is in ``config/moveit_cpp.yaml``.

3D Perception Configuration
^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you are using a perception sensor capable of generating 3D point clouds for motion planning, you can configure those settings for MoveIt.
For more information, refer to the :doc:`Perception Pipeline Tutorial </doc/examples/perception_pipeline/perception_pipeline_tutorial>`.

The default location of this file is in ``config/sensors_3d.yaml``.

Loading Configuration Parameters into Launch Files
--------------------------------------------------

To easily load parameters from MoveIt configuration packages for use in your ROS 2 launch files, MoveIt provides a ``MoveItConfigsBuilder`` utility.
To load the configuration parameters from your ``my_robot_moveit_config`` package:

.. code-block:: python

    from moveit_configs_utils import MoveItConfigsBuilder

    moveit_config = (
        MoveItConfigsBuilder("my_robot")
        .to_moveit_configs()
    )

Then, you can either use the complete set of configuration parameters when launching a node:

.. code-block:: python

    from launch_ros.actions import Node

    my_node = Node(
        package="my_package",
        executable="my_executable",
        parameters=[moveit_config.to_dict()],
    )

or you can include selected sub-components as follows:

.. code-block:: python

    from launch_ros.actions import Node

    my_node = Node(
        package="my_package",
        executable="my_executable",
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
        ],
    )

Note that the above syntax will automatically look for configuration files that match the default file naming patterns described in this document.
If you have a different naming convention, you can use the functions available in ``MoveItConfigsBuilder`` to directly set file names.
For example, to use a non-default robot description and IK solver file path, and configure planning pipelines:

.. code-block:: python

    from moveit_configs_utils import MoveItConfigsBuilder

    moveit_config = (
        MoveItConfigsBuilder("my_robot")
        .robot_description(file_path="config/my_robot.urdf.xacro")
        .robot_description_kinematics(file_path="config/my_kinematics_solver.yaml")
        .planning_pipelines(
            pipelines=["ompl", "pilz_industrial_motion_planner"],
            default_planning_pipeline="pilz_industrial_motion_planner",
        )
        .to_moveit_configs()
    )

Now that you have read this page, you should be able to better understand the launch files available throughout the MoveIt 2 tutorials, and when encountering other MoveIt configuration packages in the wild.
