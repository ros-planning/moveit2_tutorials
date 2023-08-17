"""
A launch file that starts a Jupyter notebook server and nodes that support motion planning with the MoveIt Python library.
"""
import os
import yaml
from launch import LaunchDescription
from launch.actions import ExecuteProcess, DeclareLaunchArgument
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import Node, SetParameter
from ament_index_python.packages import get_package_share_directory
from moveit_configs_utils import MoveItConfigsBuilder


def load_yaml(package_name, file_path):
    package_path = get_package_share_directory(package_name)
    absolute_file_path = os.path.join(package_path, file_path)

    try:
        with open(absolute_file_path, "r") as file:
            return yaml.safe_load(file)
    except EnvironmentError:  # parent of IOError, OSError *and* WindowsError where available
        return None


def generate_launch_description():
    start_servo = LaunchConfiguration("start_servo")

    start_servo_arg = DeclareLaunchArgument(
        "start_servo",
        default_value="false",
        description="Start the servo node",
    )

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
                "jupyter_notebook_prototyping.yaml",
            )
        )
        .to_moveit_configs()
    )

    rviz_config_file = os.path.join(
        get_package_share_directory("moveit2_tutorials"),
        "config",
        "jupyter_notebook_prototyping.rviz",
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
                output="screen",
            )
        ]

    # We can start a notebook from a launch file
    notebook_dir = os.path.join(get_package_share_directory("moveit2_tutorials"), "src")
    start_notebook = ExecuteProcess(
        cmd=["cd {} && python3 -m notebook".format(notebook_dir)],
        shell=True,
        output="screen",
    )

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

    else:
        return LaunchDescription(
            [
                start_servo_arg,
                start_notebook,
                static_tf,
                robot_state_publisher,
                rviz_node,
                ros2_control_node,
            ]
            + load_controllers
        )
