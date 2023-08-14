pick_ik Kinematics Solver
=========================

`pick_ik <https://github.com/PickNikRobotics/pick_ik>`_ is an inverse kinematics (IK) solver compatible with MoveIt 2,
developed by PickNik Robotics. It is designed to provide robust and customizable IK solutions,
offering a wide range of features.

The solver in ``pick_ik`` is a reimplementation of `bio_ik <https://github.com/TAMS-Group/bio_ik>`_,
integrating a global optimizer and a local optimizer. The global optimizer utilizes evolutionary algorithms to explore
alternative solutions within the solution space and identify global optima. Building upon the results obtained by the global optimizer,
the local optimizer applies gradient descent for iterative refinement of the solution. It takes a potential optimum solution provided
by the global optimizer as input and aims to improve its accuracy and ultimately convergence to the most optimum solution.

Getting Started
---------------
Before proceeding, please ensure that you have completed the steps outlined in the :doc:`Getting Started Guide </doc/tutorials/getting_started/getting_started>`.

Additionally, it is required to have a MoveIt configuration package specifically tailored to your robot.
This package can be created using the :doc:`MoveIt Setup Assistant </doc/examples/setup_assistant/setup_assistant_tutorial>`.

Installation
------------

From binaries
^^^^^^^^^^^^^
Make sure your ROS 2 installation is sourced and run the following command ::

    sudo apt install ros-$ROS_DISTRO-pick-ik

From source
^^^^^^^^^^^

Create a colcon workspace. ::

    export COLCON_WS=~/ws_moveit2/
    mkdir -p $COLCON_WS/src

Clone this repository in the src directory of your workspace. ::

    cd $COLCON_WS/src
    git clone -b main https://github.com/PickNikRobotics/pick_ik.git

Set up colcon mixins. ::

    sudo apt install python3-colcon-common-extensions
    sudo apt install python3-colcon-mixin
    colcon mixin add default https://raw.githubusercontent.com/colcon/colcon-mixin-repository/master/index.yaml
    colcon mixin update default

Build the workspace. ::

    cd /path/to/your/workspace
    colcon build --mixin release

Usage
-----

Using pick_ik as a Kinematics Plugin
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

You can use :doc:`MoveIt Setup Assistant </doc/examples/setup_assistant/setup_assistant_tutorial>` to create
the configuration files for your robot to use it with MoveIt, or edit the ``kinematics.yaml`` file in your
robot's config directory to use pick_ik as the IK solver. ::

    panda_arm:
        kinematics_solver: pick_ik/PickIkPlugin
        kinematics_solver_timeout: 0.05
        kinematics_solver_attempts: 3
        mode: global
        position_scale: 1.0
        rotation_scale: 0.5
        position_threshold: 0.001
        orientation_threshold: 0.01
        cost_threshold: 0.001
        minimal_displacement_weight: 0.0
        gd_step_size: 0.0001


.. note::

   You can launch a preconfigured demo with ``pick_ik`` using the following command:

   .. code-block::

      ros2 launch moveit2_tutorials demo_pick_ik.launch.py

   The command starts a demo similar to the one in :doc:`MoveIt Quickstart in RViz </doc/tutorials/quickstart_in_rviz/quickstart_in_rviz_tutorial>`
   tutorial. However, this demo specifically utilizes the robot kinematics configuration file ``kinematics_pick_ik.yaml``
   located at the ``moveit2_tutorials/doc/pick_ik/config`` directory :codedir:`here<how_to_guides/pick_ik/config/kinematics_pick_ik.yaml>`.

Parameter Description
^^^^^^^^^^^^^^^^^^^^^

For an exhaustive list of parameters, refer to the `parameters YAML file <https://github.com/PickNikRobotics/pick_ik/blob/main/src/pick_ik_parameters.yaml>`__.

Some key parameters you may want to start with are:

- ``mode``: If you choose ``local``, this solver will only do local gradient descent; if you choose ``global``,
  it will also enable the evolutionary algorithm. Using the global solver will be less performant, but if you're
  having trouble getting out of local minima, this could help you. We recommend using ``local`` for things like
  relative motion / Cartesian interpolation / endpoint jogging, and ``global`` if you need to solve for goals
  with a far-away initial condition.

- ``memetic_<property>``: All the properties that only kick in if you use the ``global`` solver.
  The key one is ``memetic_num_threads``, as we have enabled the evolutionary algorithm to solve on multiple threads.

- ``position_threshold`` / ``orientation_threshold``: Optimization succeeds only if the pose difference is less than
  these thresholds in meters and radians respectively. A ``position_threshold`` of 0.001 would mean a 1 mm accuracy and
  an ``orientation_threshold`` of 0.01 would mean a 0.01 radian accuracy.

- ``cost_threshold``: This solver works by setting up cost functions based on how far away your pose is,
  how much your joints move relative to the initial guess, and custom cost functions you can add.
  Optimization succeeds only if the cost is less than ``cost_threshold``. Note that if you're adding custom cost functions,
  you may want to set this threshold fairly high and rely on ``position_threshold`` and ``orientation_threshold`` to be your deciding factors,
  whereas this is more of a guideline.

- ``approximate_solution_position_threshold`` / ``approximate_solution_orientation_threshold``:
  When using approximate IK solutions for applications such as endpoint servoing, ``pick_ik`` may sometimes return solutions
  that are significantly far from the goal frame. To prevent issues with such jumps in solutions,
  these parameters define maximum translational and rotation displacement.
  We recommend setting this to values around a few centimeters and a few degrees for most applications.

- ``position_scale``: If you want rotation-only IK, set this to 0.0. If you want to solve for a custom ``IKCostFn``
  (which you provide in your ``setFromIK()`` call), set both ``position_scale`` and ``rotation_scale`` to 0.0.
  You can also use any other value to weight the position goal; it's part of the cost function.
  Note that any checks using ``position_threshold`` will be ignored if you use ``position_scale = 0.0``.


- ``rotation_scale``: If you want position-only IK, set this to 0.0. If you want to treat position and orientation equally,
  set this to 1.0. You can also use any value in between; it's part of the cost function. Note that any checks using ``orientation_threshold``
  will be ignored if you use ``rotation_scale = 0.0``.

- ``minimal_displacement_weight``: This is one of the standard cost functions that checks for the joint angle difference between
  the initial guess and the solution. If you're solving for far-away goals, leave it to zero or it will hike up your cost function for no reason.
  Have this to a small non-zero value (e.g., 0.001) if you're doing things like Cartesian interpolation along a path or endpoint jogging for servoing.

You can test out this solver live in RViz, as this plugin uses the `generate_parameter_library <https://github.com/PickNikRobotics/generate_parameter_library>`_
package to respond to parameter changes at every solve. This means that you can change values on the fly using the ROS 2 command-line interface, e.g.,

.. code-block::

    ros2 param set /rviz2 robot_description_kinematics.panda_arm.mode global

    ros2 param set /rviz2 robot_description_kinematics.panda_arm.minimal_displacement_weight 0.001
