Realtime Servo
===============

MoveIt Servo facilitates realtime control of your robot arm.


.. raw:: html

    <div style="position: relative; padding-bottom: 5%; height: 0; overflow: hidden; max-width: 100%; height: auto;">
        <iframe width="700px" height="400px" src="https://www.youtube.com/embed/j45Lagelpwo" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>
    </div>

MoveIt Servo accepts any of the following types of commands:

    1. Individual joint velocities.
    2. The desired velocity of end effector.
    3. The desired pose of end effector.

This enables teleoperation via a wide range of input schemes, or for other autonomous software to control the robot - in visual servoing or closed loop position control for instance.

Getting Started
---------------

If you haven't already done so, make sure you've completed the steps in :doc:`Getting Started </doc/tutorials/getting_started/getting_started>`.


Design overview
---------------

Moveit Servo consists of two main parts: The core implementation ``Servo`` which provides a C++ interface, and the ``ServoNode`` which
wraps the C++ interface and provides a ROS interface.The configuration of Servo is done through ROS parameters specified in :moveit_codedir:`servo_parameters.yaml <moveit_ros/moveit_servo/config/servo_parameters.yaml>`

In addition to the servoing capability, MoveIt Servo has some convenient features such as:

    - Checking for singularities
    - Checking for collisions
    - Motion smoothing
    - Joint position and velocity limits enforced

Singularity checking and collision checking are safety features that scale down the velocities when approaching singularities or collisions (self collision or collision with other objects).
The collision checking and smoothing are optional features that can be disabled using the ``check_collisions`` parameter and the ``use_smoothing`` parameters respectively.

The inverse kinematics is handled through either the inverse Jacobain or the robot's IK solver if one was provided.


Inverse Kinematics in Servo
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Inverse Kinematics may be handled internally by MoveIt Servo via inverse Jacobian calculations. However, you may also use an IK plugin.
To configure an IK plugin for use in MoveIt Servo, your robot config package must define one in a :code:`kinematics.yaml` file, such as the one
in the :moveit_resources_codedir:`Panda config package <panda_moveit_config/config/kinematics.yaml>`.
Several IK plugins are available :moveit_codedir:`within MoveIt <moveit_kinematics>`, as well as `externally <https://github.com/PickNikRobotics/bio_ik/tree/ros2>`_.
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
        executable="servo_node",
        parameters=[
            servo_params,
            low_pass_filter_coeff,
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics, # here is where kinematics plugin parameters are passed
        ],
    )


The above excerpt is taken from :moveit_codedir:`servo_example.launch.py <moveit_ros/moveit_servo/launch/demo_ros_api.launch.py>` in MoveIt.
In the above example, the :code:`kinematics.yaml` file is taken from the :moveit_resources_codedir:`moveit_resources </>` repository in the workspace, specifically :code:`moveit_resources/panda_moveit_config/config/kinematics.yaml`.
The actual ROS parameter names that get passed by loading the yaml file are of the form :code:`robot_description_kinematics.<group_name>.<param_name>`, e.g. :code:`robot_description_kinematics.panda_arm.kinematics_solver`.

Since :code:`moveit_servo` does not allow undeclared parameters found in the :code:`kinematics.yaml` file to be set on the Servo node, custom solver parameters need to be declared from inside your plugin code.

For example, :code:`bio_ik` defines a :code:`getROSParam()` function in `bio_ik/src/kinematics_plugin.cpp <https://github.com/PickNikRobotics/bio_ik/blob/ros2/src/kinematics_plugin.cpp#L160>`_ that declares parameters if they're not found on the Servo Node.


Thread Priority
-----------------

For best performance when controlling hardware you want the main servo loop to have as little jitter as possible. The normal linux kernel is optimized for computational throughput and therefore is not well suited for hardware control. The two easiest kernel options are the `Real-time Ubuntu 22.04 LTS Beta <https://ubuntu.com/blog/real-time-ubuntu-released>`_ or `linux-image-rt-amd64 <https://packages.debian.org/bullseye/linux-image-rt-amd64>`_ on Debian Bullseye.

If you have a realtime kernel installed, the main thread of ``ServoNode`` automatically attempts to configure ``SCHED_FIFO`` with a priority of ``40``. See more documentation at :moveit_codedir:`config/servo_parameters.yaml <moveit_ros/moveit_servo/config/servo_parameters.yaml>`.


Setup on a New Robot
--------------------

The bare minimum requirements for running MoveIt Servo with your robot include:
    1. A valid URDF and SRDF of the robot.
    2. A controller that can accept joint positions or velocities.
    3. Joint encoders that provide rapid and accurate joint position feedback.

Because the kinematics are handled by the core parts of MoveIt, it is recommended that you have a valid config package for your robot and you can run the demo launch file included with it.


Using the C++ API
------------------
This can be beneficial when there is a performance requirement to avoid the overhead of ROS communication infrastucture, or when the output generated by Servo needs to be fed into some other controller that does not have a ROS interface.

When using MoveIt Servo with the C++ interface the three input command types are ``JointJogCommand``, ``TwistCommand`` and ``PoseCommand``.
The output from Servo when using the C++ interface is ``KinematicState``, a struct containing joint names, positions, velocities and accelerations.
As given by the definitions in :moveit_codedir:`datatypes <moveit_ros/moveit_servo/include/moveit_servo/utils/datatypes.hpp>` header file.

The first step is to create a ``Servo`` instance.

.. code-block:: c++

    // Import the Servo headers.
    #include <moveit_servo/servo.hpp>
    #include <moveit_servo/utils/common.hpp>

    // The node to be used by Servo.
    rclcpp::Node::SharedPtr node = std::make_shared<rclcpp::Node>("servo_tutorial");

    // Get the Servo parameters.
    const std::string param_namespace = "moveit_servo";
    const std::shared_ptr<const servo::ParamListener> servo_param_listener =
        std::make_shared<const servo::ParamListener>(node, param_namespace);
    const servo::Params servo_params = servo_param_listener->get_params();

    // Create the planning scene monitor.
    const planning_scene_monitor::PlanningSceneMonitorPtr planning_scene_monitor =
        createPlanningSceneMonitor(node, servo_params);

    // Create a Servo instance.
    Servo servo = Servo(node, servo_param_listener, planning_scene_monitor);


Using the JointJogCommand

.. code-block:: c++

    using namespace moveit_servo;

    // Create the command.
    JointJogCommand command;
    command.joint_names = {"panda_link7"};
    command.velocities = {0.1};

    // Set JointJogCommand as the input type.
    servo.setCommandType(CommandType::JOINT_JOG);

    // Get the joint states required to follow the command.
    // This is generally run in a loop.
    KinematicState next_joint_state = servo.getNextJointState(command);

Using the TwistCommand

.. code-block:: c++

    using namespace moveit_servo;

    // Create the command.
    TwistCommand command{"panda_link0", {0.1, 0.0, 0.0, 0.0, 0.0, 0.0};

    // Set the command type.
    servo.setCommandType(CommandType::TWIST);

    // Get the joint states required to follow the command.
    // This is generally run in a loop.
    KinematicState next_joint_state = servo.getNextJointState(command);


Using the PoseCommand

.. code-block:: c++

    using namespace moveit_servo;

    // Create the command.
    Eigen::Isometry3d ee_pose = Eigen::Isometry3d::Identity(); // This is a dummy pose.
    PoseCommand command{"panda_link0", ee_pose};

    // Set the command type.
    servo.setCommandType(CommandType::POSE);

    // Get the joint states required to follow the command.
    // This is generally run in a loop.
    KinematicState next_joint_state = servo.getNextJointState(command);

The ``next_joint_state`` result can then be used for further steps in the control pipeline.

The status of MoveIt Servo resulting from the last command can be obtained by:

.. code-block:: c++

    StatusCode status = servo.getStatus();

The user can use status for higher-level decision making.

See :moveit_codedir:`moveit_servo/demos <moveit_ros/moveit_servo/demos/cpp_interface>` for complete examples of using the C++ interface.
The demos can be launched using the launch files found in :moveit_codedir:`moveit_servo/launch <moveit_ros/moveit_servo/launch>`.

    - ``ros2 launch moveit_servo demo_joint_jog.launch.py``
    - ``ros2 launch moveit_servo demo_twist.launch.py``
    - ``ros2 launch moveit_servo demo_pose.launch.py``


Using the ROS API
-----------------

To use MoveIt Servo through the ROS interface, it must be launched as a ``Node`` or ``Component`` along with the required parameters as seen :moveit_codedir:`here <moveit_ros/moveit_servo/launch/demo_ros_api.launch.py>`.

When using MoveIt Servo with the ROS interface the commands are ROS messages of the following types published to respective topics specified by the Servo parameters.

    1. ``control_msgs::msg::JointJog`` on the topic specified by the ``joint_command_in_topic`` parameter.
    2. ``geometry_msgs::msg::TwistStamped`` on the topic specified by the ``cartesian_command_in_topic`` parameter. For now, the twist message must be in the planning frame of the robot. (This will be updated soon.)
    3. ``geometry_msgs::msg::PoseStamped`` on the topic specified by the ``pose_command_in_topic`` parameter.

Twist and Pose commands require that the ``header.frame_id`` is always specified.
The output from ``ServoNode`` (the ROS interface) can either be ``trajectory_msgs::msg::JointTrajectory`` or ``std_msgs::msg::Float64MultiArray``
selected using the *command_out_type* parameter, and published on the topic specified by *command_out_topic* parameter.

The command type can be selected using the ``ServoCommandType`` service, see definition :moveit_msgs_codedir:`ServoCommandType <srv/ServoCommandType.srv>`.

From cli : ``ros2 service call /<node_name>/switch_command_type moveit_msgs/srv/ServoCommandType "{command_type: 1}"``

Programmatically:

.. code-block:: c++

        switch_input_client = node->create_client<moveit_msgs::srv::ServoCommandType>("/<node_name>/switch_command_type");
        auto request = std::make_shared<moveit_msgs::srv::ServoCommandType::Request>();
        request->command_type = moveit_msgs::srv::ServoCommandType::Request::TWIST;
        if (switch_input_client->wait_for_service(std::chrono::seconds(1)))
        {
          auto result = switch_input_client->async_send_request(request);
          if (result.get()->success)
          {
            RCLCPP_INFO_STREAM(node->get_logger(), "Switched to input type: Twist");
          }
          else
          {
            RCLCPP_WARN_STREAM(node->get_logger(), "Could not switch input to: Twist");
          }
        }

Similarly, servoing can be paused using the pause service ``<node_name>/pause_servo`` of type ``std_msgs::srv::SetBool``.

When using the ROS interface, the status of Servo is available on the topic ``/<node_name>/status``, see definition :moveit_msgs_codedir:`ServoStatus <msg/ServoStatus.msg>`.

Launch ROS interface demo: ``ros2 launch moveit_servo demo_ros_api.launch.py``.

Once the demo is running, the robot can be teleoperated through the keyboard.

Launch the keyboard demo: ``ros2 run moveit_servo servo_keyboard_input``.

An example of using the pose commands in the context of servoing to open a door can be seen in this :codedir:`example <examples/realtime_servo/src/pose_tracking_tutorial.cpp>`.
