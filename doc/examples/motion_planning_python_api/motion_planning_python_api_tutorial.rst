Motion Planning Python API
==================================

.. raw:: html

        <iframe width="560" height="315" src="https://www.youtube.com/embed/7KvF7Dj7bz0" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture" allowfullscreen></iframe>

In this tutorial, we will cover the basics of the motion planning API for ``moveit_py``.
This tutorial is broken down into the following sections:

* **Getting Started:** An outline of the tutorial setup requirements.
* **Understanding Planning Parameters:** An outline of setting parameters for supported planners.
* **Single Pipeline Planning (Default Configurations):** Planning using prespecified robot configurations.
* **Single Pipeline Planning (Robot State):** Planning using a robot state instance.
* **Single Pipeline Planning (Pose Goal):** Planning using a pose goal.
* **Single Pipeline Planning (Custom Constraints):** Planning using custom constraints.
* **Multi Pipeline Planning:** Running multiple planning pipelines in parallel.
* **Using a Planning Scene:** Adding and removing collision objects and collision checking.

:doc:`/doc/examples/moveit_cpp/moveitcpp_tutorial` and
:doc:`/doc/examples/move_group_interface/move_group_interface_tutorial`

The code for this tutorial can be found :codedir:`here in the moveit2_tutorials GitHub project<examples/motion_planning_python_api>`.

Getting Started
-----------------------------------------------
To complete this tutorial, you must have set up a workspace that includes MoveIt 2 and its corresponding tutorials.
An outline on how to set up such a workspace is provided in the :doc:`Getting Started Guide </doc/tutorials/getting_started/getting_started>`, please consult this guide for further information.

Once you have set up your workspace, you can execute the code for this tutorial by running the following command: ::

        ros2 launch moveit2_tutorials motion_planning_python_api_tutorial.launch.py

Understanding Planning Parameters
----------------------------------------------------
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
                        planning_time: 1.0

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


The first block of the configuration file sets the planning scene monitor options such as the topics that it subscribes to (Note: if you aren't familiar with the planning scene monitor, consider reviewing :doc:`this tutorial </doc/examples/planning_scene_monitor/planning_scene_monitor_tutorial>` ): ::

        planning_scene_monitor_options:
                name: "planning_scene_monitor"
                robot_description: "robot_description"
                joint_state_topic: "/joint_states"
                attached_collision_object_topic: "/moveit_cpp/planning_scene_monitor"
                publish_planning_scene_topic: "/moveit_cpp/publish_planning_scene"
                monitored_planning_scene_topic: "/moveit_cpp/monitored_planning_scene"
                wait_for_initial_state_timeout: 10.0

The second block of the configuration file sets the planning pipelines that we wish to use.
MoveIt supports multiple motion planning libraries including OMPL, Pilz Industrial Motion Planner, Stochastic Trajectory Optimization for Motion Planning (STOMP), Search-Based Planning Library (SBPL), and Covariant Hamiltonian Optimization for Motion Planning (CHOMP) to name a few.
When configuring our ``moveit_py`` node, we need to specify the configuration for planning pipelines we wish to use: ::

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

These specified parameters will be made available as ``moveit_py`` node parameters and will be leveraged at runtime when performing planning.
This is what we will investigate next.

Instantiating moveit_py and planning component
----------------------------------------------------
Before we can plan motions, we need to instantiate a ``moveit_py`` node and its derived planning component.
We will also instantiate a ``rclpy`` logger object: ::

        rclpy.init()
        logger = rclpy.logging.get_logger("moveit_py.pose_goal")

        # instantiate MoveItPy instance and get planning component
        panda = MoveItPy(node_name="moveit_py")
        panda_arm = panda.get_planning_component("panda_arm")
        logger.info("MoveItPy instance created")

Using the planning component represented by the ``panda_arm`` variable, we can begin to perform motion planning.
We also define a helper function for planning and executing motions: ::

        def plan_and_execute(
                robot,
                planning_component,
                logger,
                single_plan_parameters=None,
                multi_plan_parameters=None,
                ):
                """A helper function to plan and execute a motion."""
                # plan to goal
                logger.info("Planning trajectory")
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
                        logger.info("Executing plan")
                        robot_trajectory = plan_result.trajectory
                        robot.execute(robot_trajectory, controllers=[])
                else:
                        logger.error("Planning failed")

Single Pipeline Planning - Default Configurations
----------------------------------------------------
We start exploring the ``moveit_py`` motion planning API through executing a single planning pipeline which will plan to a predefined robot configuration (defined in the srdf file): ::

        # set plan start state using predefined state
        panda_arm.set_start_state(configuration_name="ready")

        # set pose goal using predefined state
        panda_arm.set_goal_state(configuration_name="extended")

        # plan to goal
        plan_and_execute(panda, panda_arm, logger)

Single Pipeline Planning - Robot State
----------------------------------------------------
Next, we will plan to a robot state.
Such a method is quite flexible as we can alter the robot state configuration as we wish (e.g., through setting joint values).
Here, we will use the ``set_start_state_to_current_state`` method to set the start state of the robot to its current state and the ``set_goal_state`` method to set the goal state to a random configuration.
We will then plan to the goal state and execute the plan: ::

        # instantiate a RobotState instance using the current robot model
        robot_model = panda.get_robot_model()
        robot_state = RobotState(robot_model)

        # randomize the robot state
        robot_state.set_to_random_positions()

        # set plan start state to current state
        panda_arm.set_start_state_to_current_state()

        # set goal state to the initialized robot state
        logger.info("Set goal state to the initialized robot state")
        panda_arm.set_goal_state(robot_state=robot_state)

        # plan to goal
        plan_and_execute(panda, panda_arm, logger)

Single Pipeline Planning - Pose Goal
----------------------------------------------------
Another common way to specify a goal state is via a ROS message representing the pose goal.
Here we demonstrate how to set a pose goal for the end effector of the robot: ::

        # set plan start state to current state
        panda_arm.set_start_state_to_current_state()

        # set pose goal with PoseStamped message
        pose_goal = PoseStamped()
        pose_goal.header.frame_id = "panda_link0"
        pose_goal.pose.orientation.w = 1.0
        pose_goal.pose.position.x = 0.28
        pose_goal.pose.position.y = -0.2
        pose_goal.pose.position.z = 0.5
        panda_arm.set_goal_state(pose_stamped_msg=pose_goal, pose_link="panda_link8")

        # plan to goal
        plan_and_execute(panda, panda_arm, logger)

Single Pipeline Planning - Custom Constraints
----------------------------------------------------
You can also control the output of motion planning via custom constraints.
Here we demonstrate planning to a configuration that satisfies a set of joint constraints: ::

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
                joint_model_group=panda.get_robot_model().get_joint_model_group("panda_arm"),
        )
        panda_arm.set_goal_state(motion_plan_constraints=[joint_constraint])

        # plan to goal
        plan_and_execute(panda, panda_arm, logger)

Multi Pipeline Planning
----------------------------------------------------
A recent addition to ``moveit_cpp`` and ``moveit_py`` is the ability to execute multiple planning pipelines in parallel and select the resulting motion plan amongst all generated motion plans that best satisfies your task requirements.
In previous sections, we defined a set of planning pipelines.
Here we will see how to plan in parallel with several of these pipelines: ::

        # set plan start state to current state
        panda_arm.set_start_state_to_current_state()

        # set pose goal with PoseStamped message
        panda_arm.set_goal_state(configuration_name="ready")

        # initialise multi-pipeline plan request parameters
        multi_pipeline_plan_request_params = MultiPipelinePlanRequestParameters(
                panda, ["ompl_rrtc", "pilz_lin", "chomp", "ompl_rrt_star"]
        )

        # plan to goal
        plan_and_execute(
                panda,
                panda_arm,
                logger,
                multi_plan_parameters=multi_pipeline_plan_request_params,
        )

        # execute the plan
        if plan_result:
                logger.info("Executing plan")
                panda_arm.execute()

Using a Planning Scene
----------------------------------------------------
The code for this section requires you to run a different Python file, which you can specify as follows ::

        ros2 launch moveit2_tutorials motion_planning_python_api_tutorial.launch.py example_file:=motion_planning_python_api_planning_scene.py

Interacting with a planning scene requires you to create a planning scene monitor: ::

        panda = MoveItPy(node_name="moveit_py_planning_scene")
        panda_arm = panda.get_planning_component("panda_arm")
        planning_scene_monitor = panda.get_planning_scene_monitor()

You can then add collision objects to a planning scene using the planning scene monitor's ``read_write`` context: ::

        with planning_scene_monitor.read_write() as scene:
                collision_object = CollisionObject()
                collision_object.header.frame_id = "panda_link0"
                collision_object.id = "boxes"

                box_pose = Pose()
                box_pose.position.x = 0.15
                box_pose.position.y = 0.1
                box_pose.position.z = 0.6

                box = SolidPrimitive()
                box.type = SolidPrimitive.BOX
                box.dimensions = dimensions

                collision_object.primitives.append(box)
                collision_object.primitive_poses.append(box_pose)
                collision_object.operation = CollisionObject.ADD

                scene.apply_collision_object(collision_object)
                scene.current_state.update()  # Important to ensure the scene is updated

Removing objects can be achieved similarly using the ``CollisionObject.REMOVE`` operation, or by removing all objects from the scene: ::

        with planning_scene_monitor.read_write() as scene:
                scene.remove_all_collision_objects()
                scene.current_state.update()

You can also use the ``read_only`` context of a planning scene monitor for tasks that do not require modifying the scene, such as collision checking.
For example: ::

        with planning_scene_monitor.read_only() as scene:
                robot_state = scene.current_state
                original_joint_positions = robot_state.get_joint_group_positions("panda_arm")

                # Set the pose goal
                pose_goal = Pose()
                pose_goal.position.x = 0.25
                pose_goal.position.y = 0.25
                pose_goal.position.z = 0.5
                pose_goal.orientation.w = 1.0

                # Set the robot state and check collisions
                robot_state.set_from_ik("panda_arm", pose_goal, "panda_hand")
                robot_state.update()  # required to update transforms
                robot_collision_status = scene.is_state_colliding(
                        robot_state=robot_state, joint_model_group_name="panda_arm", verbose=True
                )
                logger.info(f"\nRobot is in collision: {robot_collision_status}\n")

                # Restore the original state
                robot_state.set_joint_group_positions(
                        "panda_arm",
                        original_joint_positions,
                )
                robot_state.update()  # required to update transforms
