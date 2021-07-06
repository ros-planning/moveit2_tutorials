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
If you haven't already done so, make sure you've completed the steps in `Getting Started <../getting_started/getting_started.html>`_.

Running the Code
----------------
Open two shells. In the first shell start RViz and wait for everything to finish loading: ::

  ros2 launch moveit_resources_panda_moveit_config demo.launch.py

In the second shell, run the launch file: ::

  ros2 launch moveit2_tutorials motion_planning_pipeline_tutorial.launch.py

**Note:** Because **MoveitVisualTools** has not been ported to ROS2 this tutoral has made use of xterm and a simple prompter to help the user progress through each demo step.
To install xterm please run the following command: ::

  sudo apt-get install -y xterm


Expected Output
---------------
In RViz, we should be able to see three trajectories being replayed eventually:

 1. The robot moves its right arm to the pose goal in front of it,
 2. The robot moves its right arm to the joint goal to the side,
 3. The robot moves its right arm back to the original pose goal in front of it,

The Entire Code
---------------
The entire code can be seen :codedir:`here in the MoveIt GitHub project<motion_planning_pipeline>`.

.. tutorial-formatter:: ./src/motion_planning_pipeline_tutorial.cpp

The Launch File
---------------
The entire launch file is :codedir:`here <motion_planning_pipeline/launch/motion_planning_pipeline_tutorial.launch.py>` on GitHub. All the code in this tutorial can be compiled and run from the moveit_tutorials package that you have as part of your MoveIt setup.
