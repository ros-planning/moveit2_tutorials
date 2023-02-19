:moveit1:

..
   Once updated for MoveIt 2, remove all lines above title (including this comment and :moveit1: tag)

Motion Planning Pipeline
==================================

In MoveIt, the motion planners are setup to plan paths. However, there are often
times when we may want to pre-process the motion planning request or post-process
the planned path (e.g. for time parameterization). In such cases, we use
the planning pipeline which chains a motion planner with pre-processing and post-processing
stages. The pre and post-processing stages, called planning request adapters, can
be configured by name from the ROS parameter server. In this tutorial, we will
run you through the C++ code to instantiate and call such a planning pipeline.

Getting Started
---------------
If you haven't already done so, make sure you've completed the steps in :doc:`Getting Started </doc/tutorials/getting_started/getting_started>`.

Running the Code
----------------
Open two shells. In the first shell start RViz and wait for everything to finish loading: ::

  ros2 launch moveit2_tutorials move_group.launch.py

In the second shell, run the launch file: ::

  ros2 launch moveit2_tutorials motion_planning_pipeline_tutorial.launch.py

**Note:** This tutorial uses the **RvizVisualToolsGui** panel to step through the demo. To add this panel to RViz, follow the instructions in the :ref:`Visualization Tutorial <doc/tutorials/quickstart_in_rviz/quickstart_in_rviz_tutorial:Rviz Visual Tools>`.

After a short moment, the RViz window should appear and look similar to the one at the top of this page. To progress through each demo step either press the **Next** button in the **RvizVisualToolsGui** panel at the bottom of the screen or select **Key Tool** in the **Tools** panel at the top of the screen and then press **N** on your keyboard while RViz is focused.

Expected Output
---------------
In RViz, we should be able to see three trajectories being replayed eventually:

 1. The robot moves its right arm to the pose goal in front of it,
 2. The robot moves its right arm to the joint goal to the side,
 3. The robot moves its right arm back to the original pose goal in front of it,

The Entire Code
---------------
The entire code can be seen :codedir:`here in the MoveIt GitHub project<examples/motion_planning_pipeline>`.

.. tutorial-formatter:: ./src/motion_planning_pipeline_tutorial.cpp

The Launch File
---------------
The entire launch file is :codedir:`here <examples/motion_planning_pipeline/launch/motion_planning_pipeline_tutorial.launch.py>` on GitHub. All the code in this tutorial can be compiled and run from the moveit_tutorials package that you have as part of your MoveIt setup.
