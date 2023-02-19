Motion Planning API
==================================
.. image:: motion_planning_api_tutorial_robot_move_arm_1st.png
   :width: 700px

In MoveIt, the motion planners are loaded using a plugin infrastructure. This
allows MoveIt to load motion planners at runtime. In this example, we will
run through the C++ code required to do this.

Getting Started
---------------
If you haven't already done so, make sure you've completed the steps in :doc:`Getting Started </doc/tutorials/getting_started/getting_started>`.

Running the Demo
----------------
Open two shells. In the first shell start RViz and wait for everything to finish loading: ::

  ros2 launch moveit2_tutorials move_group.launch.py

In the second shell, run the launch file: ::

  ros2 launch moveit2_tutorials motion_planning_api_tutorial.launch.py

**Note:** This tutorial uses the **RvizVisualToolsGui** panel to step through the demo. To add this panel to RViz, follow the instructions in the :ref:`Visualization Tutorial <doc/tutorials/quickstart_in_rviz/quickstart_in_rviz_tutorial:RViz Visual Tools>`.

After a short moment, the RViz window should appear and look similar to the one at the top of this page. To progress through each demo step either press the **Next** button in the **RvizVisualToolsGui** panel at the bottom of the screen or select **Key Tool** in the **Tools** panel at the top of the screen and then press **N** on your keyboard while RViz is focused.


Expected Output
---------------
In RViz, we should be able to see four trajectories being replayed eventually:

 1. The robot moves its arm to the first pose goal,

    |A|

 2. The robot moves its arm to the joint goal,

    |B|

 3. The robot moves its arm back to the original pose goal,
 4. The robot moves its arm to a new pose goal while maintaining the end-effector level.

    |C|

.. |A| image:: motion_planning_api_tutorial_robot_move_arm_1st.png
               :width: 200px
.. |B| image:: motion_planning_api_tutorial_robot_move_arm_2nd.png
               :width: 200px
.. |C| image:: motion_planning_api_tutorial_robot_move_arm_3rd.png
               :width: 200px

The Entire Code
---------------
The entire code can be seen :codedir:`here in the moveit_tutorials GitHub project<examples/motion_planning_api>`.

.. tutorial-formatter:: ./src/motion_planning_api_tutorial.cpp

The Launch File
---------------
The entire launch file is :codedir:`here <examples/motion_planning_api/launch/motion_planning_api_tutorial.launch.py>` on GitHub. All the code in this tutorial can be compiled and run from the moveit_tutorials package.
