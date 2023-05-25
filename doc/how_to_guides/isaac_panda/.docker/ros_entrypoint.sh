#!/bin/bash

# Source ROS 2 Humble
source /opt/ros/humble/setup.bash
echo "Sourced ROS 2 Humble"

# Source the base workspace, if built
if [ -f /root/isaac_moveit_tutorial_ws/install/setup.bash ]
then
  source /root/isaac_moveit_tutorial_ws/install/setup.bash
  echo "Sourced isaac_moveit_tutorial workspace"
else
  echo "Please build the isaac_moveit_tutorial workspace with:"
  echo "colcon build"
fi

# Execute the command passed into this entrypoint
exec "$@"
