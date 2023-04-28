Kinematics Cost Functions
==================================

When querying for IK solutions for a goal pose or for a cartesian path goal, we can specify *cost functions* that will be used
to evaluate the fitness of solutions.

Getting Started
---------------
If you haven't already done so, make sure you've completed the steps in :doc:`Getting Started </doc/tutorials/getting_started/getting_started>`.

This tutorial also requires the use of `bio_ik <https://github.com/PickNikRobotics/bio_ik>`_ as the Inverse Kinematics plugin. First, clone the "ros2" branch of this repository into your workspace's **src** directory: ::

  git clone -b ros2 https://github.com/PickNikRobotics/bio_ik.git

Then, change your kinematics plugin for the Panda robot by copying the following into moveit_resources/panda_moveit_config/config/kinematics.yaml within your workspace: ::

    panda_arm:
        kinematics_solver: bio_ik/BioIKKinematicsPlugin
        kinematics_solver_search_resolution: 0.005
        kinematics_solver_timeout: 0.005
        kinematics_solver_attempts: 1
        mode: gd_c # use the gradient descent solver

After making these changes, rebuild your workspace: ::

  colcon build --mixin release

Running the Code
----------------
Open two shells. In the first shell, start RViz and wait for everything to finish loading: ::

  ros2 launch moveit2_tutorials move_group.launch.py

In the second shell, run the tutorial launch file: ::

  ros2 launch moveit2_tutorials kinematics_cost_function_tutorial.launch.py

To progress through each demo step either press the **Next** button in the **RvizVisualToolsGui** panel at the bottom of the screen or select **Key Tool** in the **Tools** panel at the top of the screen and then press **0** on your keyboard while RViz is focused.

Expected Output
---------------
In RViz, we should be able to see the following:
 1. The robot moves its arm to the pose goal to its right. The L2 norm of the joint movement with and without the cost function specified is logged in the tutorial terminal.
 2. The robot moves its arm via a straight cartesian movement to the pose goal at its left.

The Entire Code
---------------
The entire code can be seen :codedir:`here in the MoveIt GitHub project<how_to_guides/kinematics_cost_function/src/kinematics_cost_function_tutorial.cpp>`. Next, we step through the code piece by piece to explain its functionality.

.. tutorial-formatter:: ./src/kinematics_cost_function_tutorial.cpp

The Launch File
---------------
The entire launch file is :codedir:`here<how_to_guides/kinematics_cost_function/launch/kinematics_cost_function_tutorial.launch.py>` on GitHub. All the code in this tutorial can be run from the **moveit2_tutorials** package that you have as part of your MoveIt setup.
