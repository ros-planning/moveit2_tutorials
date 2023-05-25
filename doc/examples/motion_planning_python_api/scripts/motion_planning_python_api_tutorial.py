#!/usr/bin/env python3
"""
A script to outline the fundamentals of the moveit_py motion planning API.
"""

import time

# generic ros libraries
import rclpy
from rclpy.logging import get_logger

# moveit python library
from moveit.core.robot_state import RobotState
from moveit.planning import (
    MoveItPy,
    MultiPipelinePlanRequestParameters,
)


def plan_and_execute(
    robot,
    planning_component,
    logger,
    single_plan_parameters=None,
    multi_plan_parameters=None,
    sleep_time=0.0,
):
    """Helper function to plan and execute a motion."""
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

    time.sleep(sleep_time)


def main():

    ###################################################################
    # MoveItPy Setup
    ###################################################################
    rclpy.init()
    logger = get_logger("moveit_py.pose_goal")

    # instantiate MoveItPy instance and get planning component
    panda = MoveItPy(node_name="moveit_py")
    panda_arm = panda.get_planning_component("panda_arm")
    logger.info("MoveItPy instance created")

    ###########################################################################
    # Plan 1 - set states with predefined string
    ###########################################################################

    # set plan start state using predefined state
    panda_arm.set_start_state(configuration_name="ready")

    # set pose goal using predefined state
    panda_arm.set_goal_state(configuration_name="extended")

    # plan to goal
    plan_and_execute(panda, panda_arm, logger, sleep_time=3.0)

    ###########################################################################
    # Plan 2 - set goal state with RobotState object
    ###########################################################################

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
    plan_and_execute(panda, panda_arm, logger, sleep_time=3.0)

    ###########################################################################
    # Plan 3 - set goal state with PoseStamped message
    ###########################################################################

    # set plan start state to current state
    panda_arm.set_start_state_to_current_state()

    # set pose goal with PoseStamped message
    from geometry_msgs.msg import PoseStamped

    pose_goal = PoseStamped()
    pose_goal.header.frame_id = "panda_link0"
    pose_goal.pose.orientation.w = 1.0
    pose_goal.pose.position.x = 0.28
    pose_goal.pose.position.y = -0.2
    pose_goal.pose.position.z = 0.5
    panda_arm.set_goal_state(pose_stamped_msg=pose_goal, pose_link="panda_link8")

    # plan to goal
    plan_and_execute(panda, panda_arm, logger, sleep_time=3.0)

    ###########################################################################
    # Plan 4 - set goal state with constraints
    ###########################################################################

    # set plan start state to current state
    panda_arm.set_start_state_to_current_state()

    # set constraints message
    from moveit.core.kinematic_constraints import construct_joint_constraint

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
    plan_and_execute(panda, panda_arm, logger, sleep_time=3.0)

    ###########################################################################
    # Plan 5 - Planning with Multiple Pipelines simultaneously
    ###########################################################################

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
        sleep_time=3.0,
    )


if __name__ == "__main__":
    main()
