from launch import LaunchDescription
from launch_ros.actions import Node
from moveit_configs_utils import MoveItConfigsBuilder


def generate_launch_description():
    moveit_config = MoveItConfigsBuilder("moveit_resources_panda").to_dict()

    # Pilz + MTC Demo node
    pilz_mtc_demo = Node(
        package="moveit2_tutorials",
        executable="pilz_mtc",
        output="screen",
        parameters=[
            moveit_config,
        ],
    )

    return LaunchDescription([pilz_mtc_demo])
