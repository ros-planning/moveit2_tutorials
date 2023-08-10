import unittest

import launch
import launch_testing

# -*- coding: utf-8 -*-
from ament_index_python.packages import get_package_share_directory
from launch.actions import (
    IncludeLaunchDescription,
    TimerAction,
)
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node


def generate_test_description():

    # Launch the tutorial demo
    demo_path = (
        get_package_share_directory("moveit2_tutorials") + "/launch/demo.launch.py"
    )
    base_launch = IncludeLaunchDescription(
        PythonLaunchDescriptionSource(demo_path),
    )

    # Launch the test
    bringup_test = Node(
        executable=launch.substitutions.PathJoinSubstitution(
            [
                launch.substitutions.LaunchConfiguration("test_binary_dir"),
                "bringup_test",
            ]
        ),
        output="screen",
    )

    return launch.LaunchDescription(
        [
            launch.actions.DeclareLaunchArgument(
                name="test_binary_dir",
                description="Binary directory of package "
                "containing test executables",
            ),
            base_launch,
            # The test itself
            TimerAction(period=2.0, actions=[bringup_test]),
            launch_testing.actions.ReadyToTest(),
        ]
    ), {
        "bringup_test": bringup_test,
    }


class TestGTestWaitForCompletion(unittest.TestCase):
    # Waits for test to complete
    # Then waits a bit to make sure result files are generated
    def test_gtest_run_complete(self, bringup_test):
        self.proc_info.assertWaitForShutdown(bringup_test, timeout=4000.0)


@launch_testing.post_shutdown_test()
class TestGTestProcessPostShutdown(unittest.TestCase):
    # Checks if the test has been completed with acceptable exit codes
    def test_gtest_pass(self, proc_info, bringup_test):
        launch_testing.asserts.assertExitCodes(proc_info, process=bringup_test)
