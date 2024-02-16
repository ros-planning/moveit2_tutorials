# Motion planning python API tutorial example for Universal Robot
The functionality of these files should be similar as the original tutorial with the Panda robot.
The main reason to change this tutorial specifically for a UR robot is that the `ur_moveit_config` package is slightly different from the available packages for robots in `moveit_resources`, which made it not so straightforward to use `MoveItConfigsBuilder` and launch the tutorial with the UR. With a proposed updated version of `ur_moveit.launch.py`, renamed to `motion_planning_python_api_tutorial.launch.py` the correct parameters are added to the `moveit_py` node. By keeping the structure of the original `ur_moveit.launch.py` it should be fairly easy to use this with any UR robot. Instructions are worked out in more detail for a simulated UR5.


## Dependencies
This tutorial example requires [Universal Robots ROS2 Driver](https://github.com/UniversalRobots/Universal_Robots_ROS2_Driver/tree/main) 
and [Universal Robots ROS2 Description](https://github.com/UniversalRobots/Universal_Robots_ROS2_Description) 

## Simulated UR
A summary of instructions to set up a simulated UR5 is added in the folder `ur_sim` 

## UR motion planning pyton api tutorial
Launch the tutorial to see the moveit_py interface control the simulated UR robot:

Warning: Do NOT run this directly on a real robot yet, the planned trajectories result in big movements of the robot, which will likely cause collisions with a table. 

```
ros2 launch moveit2_tutorials motion_planning_python_api_tutorial_ur.launch.py ur_type:=ur5
```