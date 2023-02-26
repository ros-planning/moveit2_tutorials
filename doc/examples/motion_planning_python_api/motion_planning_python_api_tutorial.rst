Motion Planning Python API
==================================

.. raw:: html

        <iframe width="560" height="315" src="https://www.youtube.com/embed/7KvF7Dj7bz0" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

In this tutorial we cover the basics of the motion planning API for ``moveit_py``. This tutorial is broken down into the following sections:

* **Getting Started:** An outline of the tutorial setup requirements.
* **Understanding Planning Parameters:** An outline of setting parameters for supported planners.
* **Single Pipeline Planning (Default Configurations):** Planning using prespecified robot configurations.
* **Single Pipeline Planning (Robot State):** Planning using a robot state instance.
* **Single Pipeline Planning (Pose Goal):** Planning using a pose goal.
* **Single Pipeline Planning (Custom Constraints):** Planning using custom constraints.
* **Multi Pipeline Planning:** Running multiple planning pipelines in parallel.

:doc:`/doc/examples/moveit_cpp/moveitcpp_tutorial` and
:doc:`/doc/examples/move_group_interface/move_group_interface_tutorial`

The code for this tutorial can be found `here <https://github.com/peterdavidfagan/moveit2_tutorials/tree/moveit_py_motion_planning_python_api_tutorial/doc/examples/motion_planning_python_api>`_.


Getting Started
==================================
To complete this tutorial, you must have set up a workspace that includes MoveIt2 and its corresponding tutorials. An excellent outline on how to set up such a workspace is provided in the :doc:`Getting Started Guide </doc/tutorials/getting_started/getting_started>`, please consult this guide for further information.

Once you have set up your workspace, you can execute the code for this tutorial by running the following command: ::

        ros2 launch moveit2_tutorials motion_planning_python_api_tutorial.launch.py

Understanding Planning Parameters
==================================
MoveIt supports the use of multiple planning libraries out of the box and it is important that we provide settings/parameters for the planners we wish to use.

To accomplish this we specify a yaml configuration file that defines the parameters associated with our ``moveit_py`` node.

An example of such a configuration file is given below: ::

        planning_scene_monitor_options:
                name: "planning_scene_monitor"
                robot_description: "robot_description"
                joint_state_topic: "/joint_states"
                attached_collision_object_topic: "/moveit_cpp/planning_scene_monitor"
                publish_planning_scene_topic: "/moveit_cpp/publish_planning_scene"
                monitored_planning_scene_topic: "/moveit_cpp/monitored_planning_scene"
                wait_for_initial_state_timeout: 10.0

        planning_pipelines:
                pipeline_names: ["ompl", "pilz_industrial_motion_planner", "chomp", "ompl_rrt_star"]

        plan_request_params:
                planning_attempts: 1
                planning_pipeline: ompl
                max_velocity_scaling_factor: 1.0
                max_acceleration_scaling_factor: 1.0

        ompl_rrtc:
                plan_request_params:
                        planning_attempts: 1
                        planning_pipeline: ompl
                        planner_id: "RRTConnectkConfigDefault"
                        max_velocity_scaling_factor: 1.0
                        max_acceleration_scaling_factor: 1.0
                        planning_time: 0.5

        ompl_rrt_star:
                plan_request_params:
                        planning_attempts: 1
                        planning_pipeline: ompl_rrt_star # Different OMPL pipeline name!
                        planner_id: "RRTstarkConfigDefault"
                        max_velocity_scaling_factor: 1.0
                        max_acceleration_scaling_factor: 1.0
                        planning_time: 1.5

        pilz_lin:
                plan_request_params:
                        planning_attempts: 1
                        planning_pipeline: pilz_industrial_motion_planner
                        planner_id: "PTP"
                        max_velocity_scaling_factor: 1.0
                        max_acceleration_scaling_factor: 1.0
                        planning_time: 0.8

        chomp:
                plan_request_params:
                        planning_attempts: 1
                        planning_pipeline: chomp
                        max_velocity_scaling_factor: 1.0
                        max_acceleration_scaling_factor: 1.0
                        planning_time: 1.5


The first block of the configuration file sets the planning scene monitor options such as the topics that it subsribes to (Note: if you aren't familiar with the planning scene monitor, you should consider reviewing ... ): ::

        planning_scene_monitor_options:
                name: "planning_scene_monitor"
                robot_description: "robot_description"
                joint_state_topic: "/joint_states"
                attached_collision_object_topic: "/moveit_cpp/planning_scene_monitor"
                publish_planning_scene_topic: "/moveit_cpp/publish_planning_scene"
                monitored_planning_scene_topic: "/moveit_cpp/monitored_planning_scene"
                wait_for_initial_state_timeout: 10.0

The second block of the configuration file sets the planning pipelines that we wish to use. MoveIt supports multiple motion planning libraries including OMPL, PILZ industrial motion planner, Stochastic Trajectory Optimization for Motion Planning (STOMP), Search-Based Planning Library (SBPL) and Covariant Hamiltonian Optimization for Motion Planning (CHOMP) to name a few. When configuring our ``moveit_py`` node, we need to specify the configuration for planning pipelines we wish to use: ::

        planning_pipelines:
                pipeline_names: ["ompl", "pilz_industrial_motion_planner", "chomp", "ompl_rrt_star"]

For each of these named pipelines we must provide a configuration that identifies the planner to use via the planner_id and other settings such as the number of planning attempts: ::

        ompl_rrtc:
                plan_request_params:
                        planning_attempts: 1
                        planning_pipeline: ompl
                        planner_id: "RRTConnectkConfigDefault"
                        max_velocity_scaling_factor: 1.0
                        max_acceleration_scaling_factor: 1.0
                        planning_time: 0.5

        ompl_rrt_star:
                plan_request_params:
                        planning_attempts: 1
                        planning_pipeline: ompl_rrt_star
                        planner_id: "RRTstarkConfigDefault"
                        max_velocity_scaling_factor: 1.0
                        max_acceleration_scaling_factor: 1.0
                        planning_time: 1.5

        pilz_lin:
                plan_request_params:
                        planning_attempts: 1
                        planning_pipeline: pilz_industrial_motion_planner
                        planner_id: "PTP"
                        max_velocity_scaling_factor: 1.0
                        max_acceleration_scaling_factor: 1.0
                        planning_time: 0.8

        chomp:
                plan_request_params:
                        planning_attempts: 1
                        planning_pipeline: chomp
                        max_velocity_scaling_factor: 1.0
                        max_acceleration_scaling_factor: 1.0
                        planning_time: 1.5

These specified parameters will be made available as ``moveit_py`` node parameters and will be leveraged at runtime when performing planning. This is what we will investigate next.

Instantiating moveit_py and planning component
==================================================
Before we can plan motions we need to instantiate a ``moveit_py`` node and its derived planning component. We will also instantiate a rclpy logger object: ::

        rclpy.init()
        logger = rclpy.logging.get_logger("moveit_py.pose_goal")

        # instantiate MoveItPy instance and get planning component
        moveit = MoveItPy(node_name="moveit_py")
        panda_arm = moveit.get_planning_component("panda_arm")
        logger.info("MoveItPy instance created")

Using the planning component represented by the ``panda_arm`` variable we can begin to perform motion planning.

Single Pipeline Planning - Default Configurations
==================================================
We start exploring the ``moveit_py`` motion planning API through executing a single planning pipeline which will plan to a predefined robot configuration (defined in the srdf file): ::

        # set plan start state using predefined state
        panda_arm.set_start_state(configuration_name="ready")

        # set pose goal using predefined state
        panda_arm.set_goal(configuration_name="extended")

        # plan to goal
        logger.info("Planning trajectory")
        plan_result = panda_arm.plan()

        # execute the plan
        if plan_result:
                logger.info("Executing plan")
                panda_arm.execute()


Single Pipeline Planning - Robot State
==================================================
Next we will plan to a robot state, such a method is quite flexible as we can alter the robot state configuration as we wish (e.g. through setting joint values), here we will just set the robot state to a random configuration for simplicity. We will use the ``set_start_state_to_current_state`` method to set the start state of the robot to its current state and the ``set_goal`` method to set the goal state of the robot. We will then plan to the goal state and execute the plan: ::

        # instantiate a RobotState instance using the current robot model
        robot_model = moveit.get_robot_model()
        robot_state = RobotState(robot_model)

        # randomize the robot state
        robot_state.set_to_random_positions()

        # set plan start state to current state
        panda_arm.set_start_state_to_current_state()

        # set goal state to the initialized robot state
        logger.info("Set goal state to the initialized robot state")
        panda_arm.set_goal(robot_state=robot_state)

        # plan to goal
        logger.info("Planning trajectory")
        plan_result = panda_arm.plan()

        # execute the plan
        if plan_result:
                logger.info("Executing plan")
                panda_arm.execute()


Single Pipeline Planning - Pose Goal
==================================================
Another common way to specify a goal state is via a pose goal ROS message. Here we demonstrate how to set a pose goal for the end effector of the robot: ::

        # set plan start state to current state
        panda_arm.set_start_state_to_current_state()

        # set pose goal with PoseStamped message
        pose_goal = PoseStamped()
        pose_goal.header.frame_id = "panda_link0"
        pose_goal.pose.orientation.w = 1.0
        pose_goal.pose.position.x = 0.28
        pose_goal.pose.position.y = -0.2
        pose_goal.pose.position.z = 0.5
        pose_goal = {"link_name": "panda_link8", "pose": pose_goal}
        panda_arm.set_goal(pose_goal=pose_goal)

        # plan to goal
        logger.info("Planning trajectory")
        plan_result = panda_arm.plan()

        # execute the plan
        if plan_result:
                logger.info("Executing plan")
                panda_arm.execute()

Single Pipeline Planning - Custom Constraints
==================================================
You can also control the output of motion planning via custom constraints. Here we demonstrate planning to a configuration that satisfies a set of joint constraints: ::

        # set plan start state to current state
        panda_arm.set_start_state_to_current_state()

        # set plan start state to current state
        panda_arm.set_start_state_to_current_state()

        # set constraints message
        joint_values = {
                "panda_joint1": -1.0,
                "panda_joint2": 0.7,
                "panda_joint3": 0.7,
                "panda_joint4": -1.5,
                "panda_joint5": -0.7,
                "panda_joint6": 2.0,
                "panda_joint7": 0.0,
        }
        robot_state.joint_positions = joint_values
        joint_constraint = construct_joint_constraint(
                robot_state=robot_state,
                joint_model_group=moveit.get_robot_model().get_joint_model_group("panda_arm"),
        )
        panda_arm.set_goal(motion_plan_constraints=[joint_constraint])

        # plan to goal
        logger.info("Planning trajectory")
        plan_result = panda_arm.plan()

        # execute the plan
        if plan_result:
                logger.info("Executing plan")
                panda_arm.execute()

Multi Pipeline Planning
===========================
A recent addition to ``moveit_cpp`` and ``moveit_py`` is the ability to execute multiple planning pipelines in parallel and select the resulting motion plan amongst all generated motion plans that best satisfies your task requirements.
In previous sections, we defined a set of planning pipelines.
Here we will see how to plan in parallel with several of these pipelines : ::

        # set plan start state to current state
        panda_arm.set_start_state_to_current_state()

        # set pose goal with PoseStamped message
        panda_arm.set_goal(configuration_name="ready")

        # initialise multi-pipeline plan request parameters
        multi_pipeline_plan_request_params = MultiPipelinePlanRequestParameters(
                moveit, ["ompl_rrtc", "pilz_lin", "chomp", "ompl_rrt_star"]
        )

        # plan to goal
        logger.info("Planning trajectory")
        plan_result = panda_arm.plan(
                multi_plan_parameters=multi_pipeline_plan_request_params
        )

        # execute the plan
        if plan_result:
                logger.info("Executing plan")
                panda_arm.execute()
