import os
from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import ExecuteProcess
from ament_index_python.packages import get_package_share_directory
from moveit_configs_utils import MoveItConfigsBuilder


def generate_launch_description():
    moveit_config = (
        MoveItConfigsBuilder("moveit_resources_panda")
        .robot_description(file_path="config/panda.urdf.xacro")
        .trajectory_execution(file_path="config/gripper_moveit_controllers.yaml")
        .to_moveit_configs()
    )

    sqlite_database = (
        get_package_share_directory("moveit2_tutorials")
        + "/data/kitchen_panda_db.sqlite"
    )

    print(sqlite_database)

    ## BEGIN_SUB_TUTORIAL add_config
    ## * Add a dictionary with the warehouse_ros options
    warehouse_ros_config = {
        # For warehouse_ros_sqlite
        "warehouse_plugin": "warehouse_ros_sqlite::DatabaseConnection",
        "warehouse_host": sqlite_database,
        # For warehouse_ros_mongodb use the following instead
        # "warehouse_port": 33829,
        # "warehouse_host": "localhost",
        # "warehouse_plugin": "warehouse_ros_mongo::MongoDatabaseConnection",
    }
    ## END_SUB_TUTORIAL

    # Start the actual move_group node/action server
    ## BEGIN_SUB_TUTORIAL set_config_move_group
    ## * Add it to the Move Group config
    run_move_group_node = Node(
        package="moveit_ros_move_group",
        executable="move_group",
        output="screen",
        parameters=[
            moveit_config.to_dict(),
            # here
            warehouse_ros_config,
        ],
    )
    ## END_SUB_TUTORIAL

    # RViz
    rviz_config_file = (
        get_package_share_directory("moveit2_tutorials") + "/launch/move_group.rviz"
    )

    ## BEGIN_SUB_TUTORIAL set_config_rviz
    ## * and to the RViz config
    rviz_node = Node(
        package="rviz2",
        executable="rviz2",
        name="rviz2",
        output="log",
        arguments=["-d", rviz_config_file],
        parameters=[
            moveit_config.robot_description,
            moveit_config.robot_description_semantic,
            moveit_config.robot_description_kinematics,
            moveit_config.planning_pipelines,
            moveit_config.joint_limits,
            # here
            warehouse_ros_config,
        ],
    )
    ## END_SUB_TUTORIAL

    # Static TF
    static_tf = Node(
        package="tf2_ros",
        executable="static_transform_publisher",
        name="static_transform_publisher",
        output="log",
        arguments=["--frame-id", "world", "--child-frame-id", "panda_link0"],
    )

    # Publish TF
    robot_state_publisher = Node(
        package="robot_state_publisher",
        executable="robot_state_publisher",
        name="robot_state_publisher",
        output="both",
        parameters=[moveit_config.robot_description],
    )

    # ros2_control using FakeSystem as hardware
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

    # Load controllers
    load_controllers = []
    for controller in [
        "panda_arm_controller",
        "panda_hand_controller",
        "joint_state_broadcaster",
    ]:
        load_controllers += [
            ExecuteProcess(
                cmd=["ros2 run controller_manager spawner.py {}".format(controller)],
                shell=True,
                output="screen",
            )
        ]

    # Warehouse mongodb server
    ## BEGIN_SUB_TUTORIAL start_mongodb_server
    ## * Optionally, start the MongoDB server (uncomment if necessary)
    # mongodb_server_node = Node(
    #    package="warehouse_ros_mongo",
    #    executable="mongo_wrapper_ros.py",
    #    parameters=[
    #        warehouse_ros_config,
    #    ],
    #    output="screen",
    # )
    ## END_SUB_TUTORIAL

    return LaunchDescription(
        [
            rviz_node,
            static_tf,
            robot_state_publisher,
            run_move_group_node,
            ros2_control_node,
            # mongodb_server_node,
        ]
        + load_controllers
    )


## BEGIN_TUTORIAL
## CALL_SUB_TUTORIAL add_config
## CALL_SUB_TUTORIAL set_config_move_group
## CALL_SUB_TUTORIAL set_config_rviz
## CALL_SUB_TUTORIAL start_mongodb_server
## END_TUTORIAL
