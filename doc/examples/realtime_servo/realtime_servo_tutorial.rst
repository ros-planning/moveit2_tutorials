Realtime Arm Servoing
=====================

MoveIt Servo allows you to stream End Effector (EEF) velocity commands to your manipulator and have it execute them concurrently. This enables teleoperation via a wide range of input schemes, or for other autonomous software to control the robot - in visual servoing or closed loop position control for instance.

This tutorial shows how to use MoveIt Servo to send real-time servo commands to a ROS-enabled robot. Some nice features of the servo node are singularity handling and collision checking that prevents the operator from breaking the robot.

.. raw:: html

    <div style="position: relative; padding-bottom: 5%; height: 0; overflow: hidden; max-width: 100%; height: auto;">
        <iframe width="700px" height="400px" src="https://www.youtube.com/embed/MF-_XKpGefY" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>
    </div>

Getting Started
---------------
If you haven't already done so, make sure you've completed the steps in :doc:`Getting Started </doc/tutorials/getting_started/getting_started>`.

Launching a Servo Node
----------------------
MoveIt Servo can be launched as a "node component" or a standalone node. The launch file, moveit_servo/servo_example.launch.py, launches a standalone node by default but also contains a commented component node. Commands are sent via ROS topics. The commands can come from anywhere, such as a joystick, keyboard, or other controller.

This demo was written for an Xbox 1 controller, but can be easily modified to use any controller compatible with the `Joy package <https://index.ros.org/p/joy/#{DISTRO}>`_ by modifying the :moveit_codedir:`joystick_servo_example.cpp file <moveit_ros/moveit_servo/src/teleop_demo/joystick_servo_example.cpp>`.

To run the demo, make sure your controller is plugged in and can be detected by :code:`ros2 run joy joy_node`. Usually this happens automatically after plugging the controller in. Then launch with ::

    ros2 launch moveit_servo servo_example.launch.py

Make a service request to start Servo ::

    ros2 service call /servo_node/start_servo std_srvs/srv/Trigger {}

You should be able to control the arm with your controller now, with MoveIt Servo automatically avoiding singularities and collisions.

Without a Controller
^^^^^^^^^^^^^^^^^^^^

If you do not have a joystick or game controller, you can still try the demo using your keyboard. With the demo still running, in a new terminal, run ::

    ros2 run moveit2_tutorials servo_keyboard_input

You will be able to use your keyboard to servo the robot. Send Cartesian commands with arrow keys and the :code:`.` and :code:`;` keys. Change the Cartesian command frame with :code:`W` for world and :code:`E` for End-Effector. Send joint jogging commands with keys 1-7 (use :code:`R` to reverse direction)

.. raw:: html

    <video width="700px" controls="true" autoplay="true" loop="true">
        <source src="../../../_static/videos/Servo_Teleop_Demo.webm" type="video/webm">
        Teleoperation demo with controller
    </video>

Note that the controller overlay here is just for demonstration purposes and is not actually included

Introspection
-------------

Here are some tips for inspecting and/or debugging the system.

#. View the :code:`ros2_controllers` that are currently active with :code:`ros2 control list_controllers`. You will see a `JointTrajectoryController <https://github.com/ros-controls/ros2_controllers/tree/master/joint_trajectory_controller>`_ that receives the joint position commands from Servo and handles them in the simulated robot driver. The JointTrajectoryController is very flexible; it can handle any combination of position/velocity/(position-and-velocity) input. Servo is also compatible with `JointGroupPosition <https://github.com/ros-controls/ros2_controllers/tree/master/position_controllers>`_ or `JointGroupVelocity <https://github.com/ros-controls/ros2_controllers/tree/master/velocity_controllers>`_-type controllers.

#. :code:`ros2 topic echo /servo_node/status` shows the current state of the Servo node. If :code:`0` is published, all is well. The definition for all enums can be seen :moveit_codedir:`here.<moveit_ros/moveit_servo/include/moveit_servo/status_codes.h>`

#. :code:`ros2 node list` shows the following. :code:`ros2 node info` can be used to get more information about any of these nodes.

   - :code:`/joy_node` handles commands from the XBox controller

   - :code:`/moveit_servo_demo_container` holds several ancillary ROS2 "component nodes" that are placed in a container for faster intra-process communication

   - :code:`/servo_node` which does the calculations and collision checking for this demo. :code:`servo_node` may be moved into the demo container in the future

Using the C++ Interface
-----------------------
Instead of launching Servo as its own component, you can include Servo in your own nodes via the C++ interface. Sending commands to the robot is very similar in both cases, but for the C++ interface a little bit of setup for Servo is necessary. In exchange, you will be able to directly interact with Servo through its C++ API.

This basic C++ interface demo moves the robot in a predetermined way and can be launched with ::

    ros2 launch moveit2_tutorials servo_cpp_interface_demo.launch.py

An Rviz window should appear with a Panda arm and collision object. The arm will joint-jog for a few seconds before switching to a Cartesian movement. As the arm approaches the collision object, it slows and stops.

.. raw:: html

    <video width="700px" controls="true" autoplay="true" loop="true">
        <source src="../../../_static/videos/C++_Interface_Demo.webm" type="video/webm">
        Joint and Cartesian jogging with collision prevention
    </video>

Entire Code
-----------
The entire code is available :codedir:`here<examples/realtime_servo/src/servo_cpp_interface_demo.cpp>`

.. tutorial-formatter:: ./src/servo_cpp_interface_demo.cpp


Servo Overview
--------------

The following sections give some background information about MoveIt Servo and describe the first steps to set it up on your robot.

Servo includes a number of nice features:
    1. Cartesian End-Effector twist commands
    2. Joint commands
    3. Collision checking
    4. Singularity checking
    5. Joint position and velocity limits enforced
    6. Inputs are generic ROS messages

Inverse Kinematics in Servo
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Inverse Kinematics may be handled internally by MoveIt Servo via inverse Jacobian calculations. However, you may also use an IK plugin.
To configure an IK plugin for use in Servo, your robot config package must define one in a :code:`kinematics.yaml` file, such as the one
in the `Panda config package <https://github.com/ros-planning/moveit_resources/blob/master/panda_moveit_config/config/kinematics.yaml>`_.
Several IK plugins are available `within MoveIt <https://github.com/ros-planning/moveit2/tree/main/moveit_kinematics>`_,
as well as `externally <https://github.com/PickNikRobotics/bio_ik/tree/ros2>`_.
:code:`bio_ik/BioIKKinematicsPlugin` is the most common choice.

Once your :code:`kinematics.yaml` file has been populated, include it with the ROS parameters passed to the servo node in your launch file:

.. code-block:: python

    moveit_config = (
        MoveItConfigsBuilder("moveit_resources_panda")
        .robot_description(file_path="config/panda.urdf.xacro")
        .to_moveit_configs()
    )
    servo_node = Node(
        package="moveit_servo",
        executable="servo_node_main",
        parameters=[
            servo_params,
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics, # here is where kinematics plugin parameters are passed
        ],
    )


The above excerpt is taken from `servo_example.launch.py <https://github.com/ros-planning/moveit2/blob/main/moveit_ros/moveit_servo/launch/servo_example.launch.py>`_ in MoveIt.
In the above example, the :code:`kinematics.yaml` file is taken from the `moveit_resources <https://github.com/ros-planning/moveit_resources>`_ repository in the workspace, specifically :code:`moveit_resources/panda_moveit_config/config/kinematics.yaml`.
The actual ROS parameter names that get passed by loading the yaml file are of the form :code:`robot_description_kinematics.<group_name>.<param_name>`, e.g. :code:`robot_description_kinematics.panda_arm.kinematics_solver`.

Setup on a New Robot
--------------------

Preliminaries
^^^^^^^^^^^^^

The bare minimum requirements for running MoveIt Servo with your robot include:
    1. A valid URDF and SRDF of the robot
    2. A controller that can accept joint positions or velocities from a ROS topic
    3. Joint encoders that provide rapid and accurate joint position feedback

Because the kinematics are handled by the core parts of MoveIt, it is recommended that you have a valid config package for your robot and you can run the demo launch file included with it.

Input Devices
^^^^^^^^^^^^^

The two primary inputs to MoveIt Servo are Cartesian commands and joint commands. These come into Servo as `TwistStamped <http://docs.ros.org/en/api/geometry_msgs/html/msg/TwistStamped.html>`_ and `JointJog <http://docs.ros.org/en/api/control_msgs/html/msg/JointJog.html>`_ messages respectively. The source of the commands can be almost anything including: gamepads, voice commands, a SpaceNav mouse, or PID controllers (e.g. for visual servoing).

Requirements for incoming command messages, regardless of input device are:
    1. **TwistStamped and JointJog:** need a timestamp in the header that is updated when the message is published
    2. **JointJog:** must have valid joint names in the :code:`joint_names` field that correspond with the commands given in the :code:`displacements` or :code:`velocities` fields
    3. **(Optional) TwistStamped:** can provide an arbitrary :code:`frame_id` in the header that the twist will be applied to. If empty, the default from the configs is used

Servo Configs
^^^^^^^^^^^^^

The :moveit_codedir:`demo config file <moveit_ros/moveit_servo/config/panda_simulated_config.yaml>` shows the parameters needed for MoveIt Servo and is well documented.

Start with the parameters from the demo file, but some must be changed for your specific setup:
    1. :code:`robot_link_command_frame`: Update this to be a valid frame in your robot, recommended as the planning frame or EEF frame
    2. :code:`command_in_type`: Set to "unitless" if your input comes from a joystick, "speed_units" if the input will be in meters/second or radians/second
    3. :code:`command_out_topic`: Change this to be the input topic of your controller
    4. :code:`command_out_type`: Change this based on the type of message your controller needs
    5. :code:`publish_joint_positions` and :code:`publish_joint_velocities`: Change these based on what your controller needs. Note if :code:`command_out_type == std_msgs/Float64MultiArray`, only one of these can be True
    6. :code:`joint_topic`: Change this to be the joint_state topic for your arm, usually :code:`/joint_states`
    7. :code:`move_group_name`: Change this to be the name of your move group, as defined in your SRDF
    8. :code:`planning_frame`: This should be the planning frame of your group
