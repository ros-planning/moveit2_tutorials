==========
Kinematics
==========

The Kinematics Plugin
---------------------

MoveIt uses a plugin infrastructure, especially targeted towards allowing users to write their own inverse kinematics algorithms.
Forward kinematics and finding jacobians is integrated within the RobotState class itself.
The default inverse kinematics plugin for MoveIt is configured using the `KDL <https://github.com/orocos/orocos_kinematics_dynamics>`_ numerical jacobian-based solver.
This plugin is automatically configured by the MoveIt Setup Assistant.

******************
Collision Checking
******************

Collision checking in MoveIt is configured inside a Planning Scene using the ``CollisionWorld`` object.
Fortunately, MoveIt is set up so that users never really have to worry about how collision checking is happening.
Collision checking in MoveIt is mainly carried out using the `FCL <https://flexible-collision-library.github.io/>`_ package - the primary collision checking library of MoveIt.

Collision Objects
-----------------

MoveIt supports collision checking for different types of objects including:

- **Meshes** - you can use either ``.stl`` (standard triangle language) or ``.dae`` (digital asset exchange) formats to describe objects such as robot links.

- **Primitive Shapes** - e.g. boxes, cylinders, cones, spheres and planes

- **Octomap** - the ``Octomap`` object can be directly used for collision checking

Allowed Collision Matrix (ACM)
------------------------------

Collision checking is a very expensive operation often accounting for close to 90% of the computational expense during motion planning.
The ``Allowed Collision Matrix`` or ``ACM`` encodes a binary value corresponding to the need to check for collision between pairs of bodies (which could be on the robot or in the world).
If the value corresponding to two bodies is set to ``true`` in the ACM, no collision check between the two bodies will be performed.
The collision checking would not be required if, e.g., the two bodies are always so far away that they can never collide with each other.
Alternatively, the two bodies could be in contact with each other by default, in which case the collision detection should be disabled for the pair in the ACM.
