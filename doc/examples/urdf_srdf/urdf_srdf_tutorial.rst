URDF and SRDF
======================

URDF
----
MoveIt 2 starts with a URDF (Unified Robot Description Format), the native format for describing robots in ROS and ROS2. In this tutorial, you will find resources for the URDF, important tips and also a list of MoveIt 2 specific requirements.

URDF Resources
^^^^^^^^^^^^^^

* `URDF ROS Wiki Page <http://www.ros.org/wiki/urdf>`_ - The URDF ROS Wiki page is the source of most information about the URDF.
* :ros_documentation:`URDF Tutorials <Tutorials/URDF/URDF-Main.html>` - Tutorials for working with the URDF.
* `SOLIDWORKS URDF Plugin <http://www.ros.org/wiki/sw_urdf_exporter>`_ - A plugin that lets you generate a URDF directly from a SOLIDWORKS model.

**Note:** Although the documents above are written for ROS, all the documentation is valid given you change the commands from ROS to ROS2 (ie: rosrun -> ros2 run or roslaunch -> ros2 launch)

Important Tips
^^^^^^^^^^^^^^
This section contains a set of tips on making sure that the URDF that you generate can be used with MoveIt 2. Make sure you go through all these tips before starting to use MoveIt 2 with your robot.

Special Characters in Joint Names
"""""""""""""""""""""""""""""""""
Joint names should not contain any of the following special characters: -,[,],(,),

We hope to be able to get rid of these restrictions on the joint names soon.

Safety Limits
"""""""""""""
Some URDFs have safety limits set in addition to the joint limits of the robot. Here's an example of the safety controller specified for the Panda head pan joint: ::

   <safety_controller k_position="100" k_velocity="1.5" soft_lower_limit="-2.857" soft_upper_limit="2.857"/>

The "soft_lower_limit" field and the "soft_upper_limit" field specify the joint position limits for this joint. MoveIt 2 will compare these limits to the hard limits for the joint specified in the URDF and choose the limits that are more conservative.

.. note:: If the "soft_lower_limit" and the "soft_upper_limit" in the safety_controller are set to 0.0, your joint will be unable to move. MoveIt 2 relies on you to specify the correct robot model.

Collision Checking
""""""""""""""""""
MoveIt 2 uses the meshes specified in the URDF for collision checking. The URDF allows you to specify two sets of meshes separately for visualization and collision checking. In general, the visualization meshes can be detailed and pretty, but the collision meshes should be much less detailed. The number of triangles in a mesh affects the amount of time it takes to collision check a robot link. The number of triangles in the whole robot should be on the order of a few thousand.

Test your URDF
""""""""""""""
It is very important to test your URDF out and make sure things are ok. The ROS URDF packages provide a check_urdf tool. To verify your URDF using the check_urdf tool, follow the instructions `here <http://wiki.ros.org/urdf#Verification>`_.

URDF Examples
^^^^^^^^^^^^^
There are lots of URDFs available for robots using ROS.

* `URDF Examples <http://www.ros.org/wiki/urdf/Examples>`_ - A list of URDFs from the ROS community.


SRDF
----

The SRDF or Semantic Robot Description Format complement the URDF and specifies joint groups, default robot configurations, additional collision checking information, and additional transforms that may be needed to completely specify the robot's pose. The recommended way to generate a SRDF is using the MoveIt Setup Assistant.

Virtual Joints
^^^^^^^^^^^^^^
The URDF contains information only about the physical joints on the robot. Often, additional joints need to be defined to specify the pose of the root link on the robot with respect to a world coordinate system. In such cases, a virtual joint is used to specify this connection. E.g., a mobile robot like the PR2 that moves around in the plane is specified using a planar virtual joint that attaches the world coordinate frame to the frame of the robot. A fixed robot (like an industrial manipulator) should be attached to the world using a fixed joint.

Passive Joints
^^^^^^^^^^^^^^
Passive joints are unactuated joints on a robot, e.g. passive casters on a differential drive robots. They are specified separately in the SRDF to make sure that different components in the motion planning or control pipelines know that the joints cannot be directly controlled. If your robot has unactuated casters, they should be specified as passive casters.

Groups
^^^^^^
A 'Group' (sometimes called 'JointGroup' or 'Planning Group') is a central concept in MoveIt 2. MoveIt 2 always acts on a particular group. MoveIt 2 will only consider moving the joints in the group that it is planning for -- other joints are left stationary. (A motion plan where all joints in the robot may move can be achieved by creating a group containing all joints.) A group is simply a collection of joints and links. Each group can be specified in one of several different ways:

Collection of Joints
""""""""""""""""""""
A group can be specified as a collection of joints. All the child links of each joint are automatically included in the group.

Collection of Links
"""""""""""""""""""
A group can also be specified as a collection of links. All the parent joints of the links are also included in the group.

Serial Chain
""""""""""""
A serial chain is specified using the base link and the tip link. The tip link in a chain is the child link of the last joint in the chain. The base link in a chain is the parent link for the first joint in the chain.

Collection of Sub-Groups
""""""""""""""""""""""""
A group can also be a collection of groups. E.g., you can define left_arm and right_arm as two groups and then define a new group called both_arms that includes these two groups.

End-Effectors
^^^^^^^^^^^^^
Certain groups in a robot can be given a special designation as an end-effector. An end-effector is typically connected to another group (like an arm) through a fixed joint. Note that when specifying groups that are end-effectors, it's important to make sure that there are no common links between the end-effector and the parent group it is connected to.

Self-Collisions
^^^^^^^^^^^^^^^
The Default Self-Collision Matrix Generator (part of Setup Assistant) searches for pairs of links on the robot that can safely be disabled from collision checking, decreasing motion planning processing time. These pairs of links are disabled when they are always in collision, never in collision, in collision in the robot's default position or when the links are adjacent to each other on the kinematic chain. The sampling density specifies how many random robot positions to check for self collision. Higher densities require more computation time while lower densities have a higher possibility of disabling pairs that should not be disabled. The default value is 10,000 collision checks. Collision checking is done in parallel to decrease processing time.

Robot Poses
^^^^^^^^^^^
The SRDF can also store fixed configurations of the robot. A typical example of the SRDF in this case is in defining a HOME position for a manipulator. The configuration is stored with a string id, which can be used to recover the configuration later.

SRDF Documentation
^^^^^^^^^^^^^^^^^^
For information about the syntax for the SRDF, read more details on the `ROS SRDF Wiki page <http://www.ros.org/wiki/srdf>`_.

Loading the URDF and SRDF
-------------------------
All the components of MoveIt that use the :cpp_api:`RobotModel <moveit::core::RobotModel>` need to have access to the URDF and SRDF to function properly. In ROS 1, this was accomplished by loading the XML of each into a string parameter (``/robot_description`` and ``/robot_description_semantic`` respectively) into the global parameter server. ROS 2 does not have a global parameter server, so making sure all the appropriate nodes have access requires a little more work.

Launch File Specification
^^^^^^^^^^^^^^^^^^^^^^^^^
One option is to set the parameters for each node that requires them, which is typically done using a launch file.

Loading the URDF often uses xacro, and so loading it looks like


.. code-block:: python

    from launch_ros.parameter_descriptions import ParameterValue
    from launch.substitutions import Command

    robot_description = ParameterValue(Command(['xacro ', PATH_TO_URDF]),
                                       value_type=str)

Meanwhile, the SRDF must be read in explicitly.

.. code-block:: python

    with open(PATH_TO_SRDF, 'r') as f:
        semantic_content = f.read()

Then the values must be loaded into EACH node.

.. code-block:: python

    move_group_node = Node(package='moveit_ros_move_group', executable='move_group',
                           output='screen',
                           parameters=[{
                                'robot_description': robot_description,
                                'robot_description_semantic': semantic_content,
                                # More params
                           }],
                           )

String Topic Specification
^^^^^^^^^^^^^^^^^^^^^^^^^^
The other approach is to use publish the two strings as topics. This pattern is already done with the `Robot State Publisher <https://github.com/ros/robot_state_publisher/blob/37aff2034b58794b78f1682c8fab4d609f5d2e29/src/robot_state_publisher.cpp#L136>`_ which publishes a ``std_msgs/msg/String`` message on the ``/robot_description`` topic. This can be done in the launch file:

.. code-block:: python

    rsp_node = Node(package='robot_state_publisher',
                    executable='robot_state_publisher',
                    respawn=True,
                    output='screen',
                    parameters=[{
                        'robot_description': robot_description,
                        'publish_frequency': 15.0
                    }]
                    )

You can also tell MoveIt nodes to publish the topic as well.

.. code-block:: python

    move_group_node = Node(package='moveit_ros_move_group', executable='move_group',
                           output='screen',
                           parameters=[{
                                'robot_description': robot_description,
                                'publish_robot_description': True,
                                # More params
                           }],
                           )

Publishing the robot description as a topic only needs to be done once, not in each node that requires the description.

Similarly, we can also publish the SRDF as a ``std_msgs/msg/String`` message. This requires that one node have the parameter set in the launch file, with the additional parameter ``publish_robot_description_semantic`` set to True.

.. code-block:: python

    move_group_node = Node(package='moveit_ros_move_group', executable='move_group',
                           output='screen',
                           parameters=[{
                                'robot_description_semantic': semantic_content,
                                'publish_robot_description_semantic': True,
                                # More params
                           }],
                           )

Then all of the other nodes may subscribe to the string message that gets published.

Under the Hood: RDFLoader
^^^^^^^^^^^^^^^^^^^^^^^^^
In many places in the MoveIt code, the robot description and semantics are loaded using the :moveit_codedir:`RDFLoader<moveit_ros/planning/rdf_loader/include/moveit/rdf_loader/rdf_loader.h>`
class, which will attempt to read the parameters from the node, and if that fails, will attempt to subscribe to the String topic for a short period of time. If both methods fail to get the parameter, then a warning will be printed to the console.
