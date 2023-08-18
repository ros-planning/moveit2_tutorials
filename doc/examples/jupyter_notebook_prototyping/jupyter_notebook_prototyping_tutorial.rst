Jupyter Notebook Prototyping
==================================
.. raw:: html

        <iframe width="560" height="315" src="https://www.youtube.com/embed/7KvF7Dj7bz0" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

In this tutorial you will learn how to use Jupyter notebooks with the MoveIt 2 Python API. This tutorial is broken down into the following sections:

* **Getting Started:** An outline of the tutorial setup requirements.
* **Understanding the launch file:** An outline of the launch file specification.
* **Notebook Setup:** Notebook imports and configuration.
* **Motion Planning Example:** An example of using the ``moveit_py`` API to plan a motion.
* **Teleoperation Example:** An example of using the ``moveit_py`` API to teleoperate a robot with a joystick.

The code for this tutorial can be found `here <https://github.com/peterdavidfagan/moveit2_tutorials/tree/moveit_py_notebook_tutorial/doc/examples/jupyter_notebook_prototyping>`_.

Getting Started
---------------
To complete this tutorial, you must have set up a colcon workspace that includes MoveIt2 and its corresponding tutorials. An excellent outline on how to set up such a workspace is provided in the :doc:`Getting Started Guide </doc/tutorials/getting_started/getting_started>`.

Once you have set up your workspace, you can execute the code for this tutorial by running the following command (the servo section of this tutorial requires a PS4 Dualshock, if you don't have one consider setting this parameter to false): ::

        ros2 launch moveit2_tutorials jupyter_notebook_prototyping.launch.py start_servo:=true

+ This will launch the nodes necessary to complete this tutorial.

+ Importantly, it will also launch a jupyter notebook server that you can connect to in order to run the code in this tutorial.

+ If your browser doesn't automatically open the jupyter notebook interface you can connect to the notebook server by navigating to http://localhost:8888 in your browser.

+ You will be prompted to enter a token, which is printed to the terminal when you launch the launch file, and you can enter this token to connect to the notebook server.

+ There will also be a URL that contains the token printed in the terminal output, so you can alternatively use this URL directly to connect to the notebook server without manually entering the token.

Once you have completed these steps you are ready to progress with the tutorial. Before executing the notebook code a brief outline of the launch file specification is provided to ensure understanding of how to launch a notebook instance.


Understanding the launch file
--------------------------------
The difference between the `launch file <https://github.com/peterdavidfagan/moveit2_tutorials/blob/moveit_py_notebook_tutorial/doc/examples/jupyter_notebook_prototyping/launch/jupyter_notebook_prototyping.launch.py>`_ used in this tutorial when compared with other tutorials is the starting of a jupyter notebook server.
We will briefly review common launch file code and focus mainly on starting the notebook server.

Importing required packages: ::

        import os
        import yaml
        from launch import LaunchDescription
        from launch.actions import ExecuteProcess, DeclareLaunchArgument
        from launch.substitutions import LaunchConfiguration
        from launch_ros.actions import Node, SetParameter
        from ament_index_python.packages import get_package_share_directory
        from moveit_configs_utils import MoveItConfigsBuilder

Define a utility to load yaml files: ::

        def load_yaml(package_name, file_path):
                package_path = get_package_share_directory(package_name)
                absolute_file_path = os.path.join(package_path, file_path)

                try:
                        with open(absolute_file_path, 'r') as file:
                        return yaml.safe_load(file)
                except EnvironmentError:  # parent of IOError, OSError *and* WindowsError where available
                        return None


Define a launch argument to start the servo node: ::

        start_servo = LaunchConfiguration('start_servo')

        start_servo_arg = DeclareLaunchArgument(
                'start_servo',
                default_value='false',
                description='Start the servo node.')

Define our MoveIt configuration, this step will also be important for later when configuring our notebook. ::

        moveit_config = (
                MoveItConfigsBuilder(
                        robot_name="panda", package_name="moveit_resources_panda_moveit_config"
                )
                .robot_description(file_path="config/panda.urdf.xacro")
                .trajectory_execution(file_path="config/gripper_moveit_controllers.yaml")
                .moveit_cpp(
                        file_path=os.path.join(
                                get_package_share_directory("moveit2_tutorials"),
                                "config",
                                "jupyter_notebook_prototyping.yaml"
                        )
                )
                .to_moveit_configs()
        )

Once our MoveIt configuration is defined we start the following set of nodes:

* **rviz_node:** starts rviz2 for visualization purposes.
* **static_tf:** publishes the static transforms between the world frame and panda base frame.
* **robot_state_publisher:** publishes updated robot state information (transforms).
* **ros2_control_node:** used to control groups of joints.

::

        rviz_config_file = os.path.join(
                get_package_share_directory("moveit2_tutorials"),
                "config", "jupyter_notebook_prototyping.rviz",
        )
        rviz_node = Node(
                package="rviz2",
                executable="rviz2",
                name="rviz2",
                output="log",
                arguments=["-d", rviz_config_file],
                parameters=[
                        moveit_config.robot_description,
                        moveit_config.robot_description_semantic,
                ],
        )

        static_tf = Node(
                package="tf2_ros",
                executable="static_transform_publisher",
                name="static_transform_publisher",
                output="log",
                arguments=["--frame-id", "world", "--child-frame-id", "panda_link0"],
        )

        robot_state_publisher = Node(
                package="robot_state_publisher",
                executable="robot_state_publisher",
                name="robot_state_publisher",
                output="both",
                parameters=[moveit_config.robot_description],
        )

        ros2_controllers_path = os.path.join(
                get_package_share_directory("moveit_resources_panda_moveit_config"),
                "config",
                "ros2_controllers.yaml",
        )
        ros2_control_node = Node(
                package="controller_manager",
                executable="ros2_control_node",
                parameters=[moveit_config.robot_description, ros2_controllers_path],
                output="both",
        )

        load_controllers = []
        for controller in [
                "panda_arm_controller",
                "panda_hand_controller",
                "joint_state_broadcaster",
        ]:
                load_controllers += [
                        ExecuteProcess(
                        cmd=["ros2 run controller_manager spawner {}".format(controller)],
                        shell=True,
                        output="screen",)
                        ]

Having defined the setup for each of these nodes, we also define a process that starts our jupyter notebook server: ::

        notebook_dir = os.path.join(get_package_share_directory("moveit2_tutorials"), "src")
        start_notebook = ExecuteProcess(
                cmd=["cd {} && python3 -m notebook".format(notebook_dir)],
                shell=True,
                output="screen",
        )


If we wish to start servo we also define a joy and servo node. Finally we return the ``LaunchDescription``: ::

        if start_servo:
                servo_yaml = load_yaml("moveit_servo", "config/panda_simulated_config.yaml")
                servo_params = {"moveit_servo": servo_yaml}

                joy_node = Node(
                        package="joy",
                        executable="joy_node",
                        name="joy_node",
                        output="screen",
                )

                servo_node = Node(
                        package="moveit_servo",
                        executable="servo_node_main",
                        parameters=[
                                servo_params,
                                moveit_config.robot_description,
                                moveit_config.robot_description_semantic,
                                moveit_config.robot_description_kinematics,
                        ],
                        output="screen",
                )

                return LaunchDescription(
                        [
                        start_servo_arg,
                        start_notebook,
                        static_tf,
                        robot_state_publisher,
                        rviz_node,
                        ros2_control_node,
                        joy_node,
                        servo_node,
                ]
                + load_controllers
                )

If we don't start servo, we return a launch description that includes all of the other nodes and processes that we have defined: ::

        return LaunchDescription(
                [
                start_servo_arg,
                static_tf,
                robot_state_publisher,
                rviz_node,
                ros2_control_node,
                start_notebook,
                ]
                + load_controllers
                )

Notebook Setup
--------------
Now that we have launched our jupyter notebook server we can begin to execute the code in the notebook. The first step is to import the required packages: ::

        import os
        import sys
        import yaml
        import rclpy
        import numpy as np

        # message libraries
        from geometry_msgs.msg import PoseStamped, Pose

        # moveit_py
        from moveit.planning import MoveItPy
        from moveit.core.robot_state import RobotState

        # config file libraries
        from moveit_configs_utils import MoveItConfigsBuilder
        from ament_index_python.packages import get_package_share_directory

Once we have imported the required packages we need to define our ``moveit_py`` node configuration. We do this through using the ``MoveItConfigsBuilder`` as follows: ::

        moveit_config = (
                MoveItConfigsBuilder(robot_name="panda", package_name="moveit_resources_panda_moveit_config")
                .robot_description(file_path="config/panda.urdf.xacro")
                .trajectory_execution(file_path="config/gripper_moveit_controllers.yaml")
                .moveit_cpp(
                        file_path=os.path.join(
                                get_package_share_directory("moveit2_tutorials"),
                                "config",
                                "jupyter_notebook_prototyping.yaml",
                        )
                )
                .to_moveit_configs()
        ).to_dict()

where we convert the generated configuration instance to a dictionary so we can use it to initialise our ``moveit_py`` node. Finally we initialise a ``moveit_py`` node: ::

        # initialise rclpy (only for logging purposes)
        rclpy.init()

        # instantiate moveit_py instance and a planning component for the panda_arm
        panda = MoveItPy(node_name="moveit_py", config_dict=moveit_config)
        panda_arm = panda.get_planning_component("panda_arm")

Motion Planning Example
-----------------------
First we create a helper function that we will use later when planning and executing planned trajectories: ::

        def plan_and_execute(
                robot,
                planning_component,
                single_plan_parameters=None,
                multi_plan_parameters=None,
        ):
                """A helper function to plan and execute a motion."""
                # plan to goal
                if multi_plan_parameters is not None:
                        plan_result = planning_component.plan(
                                multi_plan_parameters=multi_plan_parameters
                        )
                elif single_plan_parameters is not None:
                        plan_result = planning_component.plan(
                        single_plan_parameters=single_plan_parameters
                )
                else:
                        plan_result = planning_component.plan()

                # execute the plan
                if plan_result:
                        robot_trajectory = plan_result.trajectory
                        robot.execute(robot_trajectory, controllers=[])
                else:
                        print("Planning failed")

We can start by demonstrating the planning and execution of a simple motion from within the notebook: ::

        # set plan start state using predefined state
        panda_arm.set_start_state("ready")

        # set pose goal using predefined state
        panda_arm.set_goal_state(configuration_name = "extended")

        # plan to goal
        plan_and_execute(panda, panda_arm)

We can perform motion planning interactively (see the motion planning tutorial for further details of the motion planning API). Suppose we are developing our code and we make a mistake such as follows: ::

        # set plan start state using predefined state
        panda_arm.set_start_state("ready") # This conflicts with the current robot configuration and will cause an error

        # set goal using a pose message this time
        pose_goal = PoseStamped()
        pose_goal.header.frame_id = "panda_link0"
        pose_goal.pose.orientation.w = 1.0
        pose_goal.pose.position.x = 0.28
        pose_goal.pose.position.y = -0.2
        pose_goal.pose.position.z = 0.5
        panda_arm.set_goal_state(pose_stamped_msg = pose_goal, pose_link = "panda_link8")

        # plan to goal
        plan_and_execute(panda, panda_arm)

Since we are using a notebook this mistake is easy to rectify without having to recompile any files. Simply edit the above notebook to match the below and rerun the cell: ::

        # set plan start state using predefined state
        panda_arm.set_start_state_to_current_state()

        # set goal using a pose message this time
        pose_goal = PoseStamped()
        pose_goal.header.frame_id = "panda_link0"
        pose_goal.pose.orientation.w = 1.0
        pose_goal.pose.position.x = 0.28
        pose_goal.pose.position.y = -0.2
        pose_goal.pose.position.z = 0.5
        panda_arm.set_goal_state(pose_stamped_msg = pose_goal, pose_link = "panda_link8")

        # plan to goal
        plan_and_execute(panda, panda_arm)

Teleoperation Example
---------------------

One may also want to perform live teleoperation of their robot. With the Python API it is possible to interactively start/stop teleoperation without shutting down and subsequently relaunching all processes. In this example, we are going to show how this is possible with notebooks through a motivating example of teleoperating the robot, performing motion planning and teleoperating the robot again.

For this section you will need a device that support teleoperation with ``moveit_py``, in this case we leverage the PS4 dualshock controller.

To start teleoperating the robot we instantiate the PS4 dualshock controller as a teleop device. ::

        from moveit.servo_client.devices.ps4_dualshock import PS4DualShockTeleop

        # instantiate the teleoperating device
        ps4 = PS4DualShockTeleop(ee_frame_name="panda_link8")

        # start teleloperating the robot
        ps4.start_teleop()

If we want to perform motion planning to bring the robot back to its default configuration, we simply stop teleoperating the robot and leverage the existing motion planning API as demonstrated below: ::

        # stop teleoperating the robot
        ps4.stop_teleop()

        # plan and execute
        # set plan start state using predefined state
        panda_arm.set_start_state_to_current_state()

        # set pose goal using predefined state
        panda_arm.set_goal_state(configuration_name = "ready")

        # plan to goal
        plan_and_execute(panda, panda_arm)

This brings the robot back to its default configuration, from this configuration we can once again start teleoperating the robot: ::

        ps4.start_teleop()
