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
If you haven't already done so, make sure you've completed the steps in `Getting Started <../getting_started/getting_started.html>`_.

Launching a Standalone Servo Node
---------------------------------
MoveIt Servo provides a component node :code:`moveit_servo::ServoServer` which allows you to send commands via ROS topics. The commands can come from anywhere, such as a joystick, keyboard, or other controller.

This demo was written for an Xbox 1 controller, but can be easily modified to use any controller compatible with the `Joy package <https://index.ros.org/p/joy/#{DISTRO}>`_ by modifying the `joystick_servo_example.cpp file <https://github.com/ros-planning/moveit2/blob/foxy/moveit_ros/moveit_servo/src/teleop_demo/joystick_servo_example.cpp>`_

To run the demo, make sure your controller is plugged in and can be detected by :code:`ros2 run joy joy_node`. Usually this happens automatically after plugging the controller in. Then launch with ::

    ros2 launch moveit2_tutorials servo_teleop.launch.py

You should be able to control the arm with your controller now, with MoveIt Servo automatically avoiding singularities and collisions.

Without a Controller
^^^^^^^^^^^^^^^^^^^^

If you do not have a controller, you can still try the demo using your keyboard. With the demo still running, in a new terminal, run ::

    ros2 run moveit2_tutorials servo_keyboard_input

You will be able to use your keyboard to servo the robot. Send Cartesian commands with arrow keys and the :code:`.` and :code:`;` keys. Change the Cartesian command frame with :code:`W` for world and :code:`E` for End-Effector. Send joint jogging commands with keys 1-7 (use :code:`R` to reverse direction)

Expected Output
---------------
.. raw:: html

    <video width="700px" controls="true" autoplay="true" loop="true">
        <source src="../../_static/videos/Servo_Teleop_Demo.webm" type="video/webm">
        Teleoperation demo with controller
    </video>

Note that the controller overlay here is just for demonstration purposes and is not actually included

Using the C++ Interface
-----------------------
Instead of launching Servo as its own component, you can include Servo in your own nodes via the C++ interface. Sending commands to the robot is very similar in both cases, but for the C++ interface a little bit of setup for Servo is necessary. In exchange, you will be able to directly interact with Servo through its C++ API.

This basic C++ interface demo moves the robot in a predetermined way and can be launched with ::

    ros2 launch moveit2_tutorials servo_cpp_interface_demo.launch.py

An Rviz window should appear with a Panda arm and collision object. The arm will joint-jog for a few seconds before switching to a Cartesian movement. As the arm approaches the collision object, it slows and stops.

Expected Output
---------------
.. raw:: html

    <video width="700px" controls="true" autoplay="true" loop="true">
        <source src="../../_static/videos/C++_Interface_Demo.webm" type="video/webm">
        Joint and Cartesian jogging with collision prevention
    </video>

Entire Code
-----------
The entire code is available :codedir:`here<realtime_servo/src/servo_cpp_interface_demo.cpp>`

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

The `demo config file <https://github.com/ros-planning/moveit2/blob/foxy/moveit_ros/moveit_servo/config/panda_simulated_config.yaml>`_ shows the parameters needed for MoveIt Servo and is well documented.

Start with the parameters from the demo file, but some must be changed for your specific setup:
    1. :code:`robot_link_command_frame`: Update this to be a valid frame in your robot, recommended as the planning frame or EEF frame
    2. :code:`command_in_type`: Set to "unitless" if your input comes from a joystick, "speed_units" if the input will be in meters/second or radians/second
    3. :code:`command_out_topic`: Change this to be the input topic of your controller
    4. :code:`command_out_type`: Change this based on the type of message your controller needs
    5. :code:`publish_joint_positions` and :code:`publish_joint_velocities`: Change these based on what your controller needs. Note if :code:`command_out_type == std_msgs/Float64MultiArray`, only one of these can be True
    6. :code:`joint_topic`: Change this to be the joint_state topic for your arm, usually :code:`/joint_states`
    7. :code:`move_group_name`: Change this to be the name of your move group, as defined in your SRDF
    8. :code:`planning_frame`: This should be the planning frame of your group
