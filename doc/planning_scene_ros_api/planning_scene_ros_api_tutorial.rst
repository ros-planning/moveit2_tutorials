Planning Scene ROS API
==================================

In this tutorial, we will examine the use of planning scene diffs to perform
two operations:

 * Adding and removing objects into the world
 * Attaching and detaching objects to the robot

Getting Started
---------------
If you haven't already done so, make sure you've completed the steps in `Getting Started <../getting_started/getting_started.html>`_.

**Note:** Because **MoveitVisualTools** has not been ported to ROS2 this tutorial has made use of xterm and a simple prompter to help the user progress through each demo step.
To install xterm please run the following command: ::

  sudo apt-get install -y xterm

Running the code
----------------
Open two shells. In the first shell start RViz and wait for everything to finish loading: ::

  ros2 launch moveit_resources_panda_moveit_config demo.launch.py

In the second shell, run the launch file for this demo: ::

  ros2 launch moveit2_tutorials planning_scene_ros_api_tutorial.launch.py

**Note:** This tutorial uses the **RvizVisualToolsGui** panel to step through the demo. To add this panel to RViz, follow the instructions in the `Visualization Tutorial <../quickstart_in_rviz/quickstart_in_rviz_tutorial.html#rviz-visual-tools>`_.

After a short moment, the RViz window should appear and look similar to `Visualization Tutorial <../quickstart_in_rviz/quickstart_in_rviz_tutorial.html#rviz-visual-tools>`_. To progress through each demo step either press the **Next** button in the **RvizVisualToolsGui** panel at the bottom of the screen or select **Key Tool** in the **Tools** panel at the top of the screen and then press **N** on your keyboard while RViz is focused.

Expected Output
---------------
In RViz, you should be able to see the following:
 * Object appears in the planning scene.
 * Object gets attached to the robot.
 * Object gets detached from the robot.
 * Object is removed from the planning scene.

The entire code
---------------
The entire code can be seen :codedir:`here in the MoveIt GitHub project<planning_scene_ros_api>`.

.. tutorial-formatter:: ./src/planning_scene_ros_api_tutorial.cpp

..
  TODO(JafarAbdi): Add the launch file section back (see https://github.com/ros-planning/moveit_tutorials/blob/master/doc/planning_scene_ros_api/planning_scene_ros_api_tutorial.rst#the-launch-file)
