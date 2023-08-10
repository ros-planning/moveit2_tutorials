Using CHOMP Planner
===================

.. image:: chomp.png
   :width: 700px

Covariant Hamiltonian Optimization for Motion Planning (CHOMP) is a gradient-based trajectory optimization procedure that makes many everyday motion planning problems both simple and trainable (Ratliff et al., 2009c). While most high-dimensional motion planners separate trajectory generation into distinct planning and optimization stages, this algorithm capitalizes on covariant gradient and functional gradient approaches to the optimization stage to design a motion planning algorithm based entirely on trajectory optimization. Given an infeasible naive trajectory, CHOMP reacts to the surrounding environment to quickly pull the trajectory out of collision while simultaneously optimizing dynamic quantities such as joint velocities and accelerations. It rapidly converges to a smooth collision-free trajectory that can be executed efficiently on the robot. `More info <http://www.nathanratliff.com/thesis-research/chomp>`_

Getting Started
---------------
If you haven't already done so, make sure you've completed the steps in :doc:`Getting Started </doc/tutorials/getting_started/getting_started>`.

You should also have gone through the steps in :doc:`Visualization with MoveIt RViz Plugin </doc/tutorials/quickstart_in_rviz/quickstart_in_rviz_tutorial>`

Prerequisites
--------------
To use CHOMP with your robot you must have a MoveIt configuration package for your robot. For example, if you have a Panda robot, it's called ``panda_moveit_config`` and can be found :moveit_resources_codedir:`here <panda_moveit_config/>`. These are typically configured using the :doc:`MoveIt Setup Assistant </doc/examples/setup_assistant/setup_assistant_tutorial>`.

Using CHOMP with Your Robot
---------------------------
**Note:** If you plan to use the ``panda_moveit_config`` package from the `ros-planning/moveit_resources <https://github.com/ros-planning/moveit_resources/tree/ros2>`_ repository, these steps are already done for you and you can skip this section. Otherwise, to add the configuration for your robot you must:

#. Create a :codedir:`chomp_demo.launch.py<examples/chomp_planner/launch/chomp_demo.launch.py>` file in the launch directory for your MoveIt config package.
#. Modify all references to Panda in ``chomp_demo.launch.py`` to point to your custom configuration instead
#. Ensure you have included a  :moveit_resources_codedir:`chomp_planning.yaml <panda_moveit_config/config/chomp_planning.yaml>` file in the config directory of your MoveIt config package.
#. Open ``chomp_planning.yaml`` in your favorite editor and change ``animate_endeffector_segment: "panda_rightfinger"`` to the appropriate link for your robot. Feel free to modify any parameters you think may better suit your needs.

Running the Demo
----------------
If you have the ``panda_moveit_config`` from the `ros-planning/moveit_resources <https://github.com/ros-planning/moveit_resources/tree/ros2>`_  repository along with ``moveit2_tutorials`` you can run the demo using: ::

  ros2 launch moveit2_tutorials chomp_demo.launch.py rviz_tutorial:=True

Note: For convenience we have provided an RViz configuration you may use, but setting ``rviz_tutorial`` to ``False`` or simply omitting it will allow you to set up RViz according to your personal preferences.

Adding Obstacles to the Scene
+++++++++++++++++++++++++++++
To add obstacles to the scene, we can use :codedir:`this node<examples/collision_environments/src/collision_scene_example.cpp>` to create a scene with obstacles.

To run the CHOMP planner with obstacles, open a second shell. In the first shell (if you closed the one from from the previous step) start RViz and wait for everything to finish loading: ::

  ros2 launch moveit2_tutorials chomp_demo.launch.py rviz_tutorial:=True

In the second shell, run the command: ::

  ros2 run moveit2_tutorials collision_scene_example

Next, in RViz, select CHOMP in the MotionPlanning panel under the Context tab. Set the desired start and goal states by moving the end-effector around with the marker and then click on the Plan button under the Planning tab in the MotionPlanning panel to start planning. The planner will now attempt to find a feasible solution between the given start and end position.

Modifying the parameters for CHOMP
-----------------------------------------
CHOMP has some optimization parameters associated with it. These can be modified for the given environment/robot you are working with and is normally present in the :moveit_resources_codedir:`chomp_planning.yaml <panda_moveit_config/config/chomp_planning.yaml>` file in the config folder of the robot you are working with. If this file does not exist for your robot, you can create it and set the parameter values as you want. The following may provide some insight on what the parameters in ``chomp_planning.yaml`` are used for:

- *planning_time_limit*: Maximum amount of time the optimizer can take to find a solution before terminating.

- *max_iterations*: Maximum number of iterations that the planner can take to find a good solution during optimization.

- *max_iterations_after_collision_free*: Maximum number of iterations to be performed after a collision-free path is found.

- *smoothness_cost_weight*:  Weight of smoothness in the final cost function for CHOMP to optimize.

- *obstacle_cost_weight*: Weight given to obstacles for the final cost CHOMP optimizes over. e.g., 0.0 would have obstacles to be ignored, 1.0 would be a hard constraint.

- *learning_rate*: The rate used by the optimizer to find the local / global minima while reducing the total cost.

- *smoothness_cost_velocity, smoothness_cost_acceleration, smoothness_cost_jerk*: Variables associated with the cost in velocity, acceleration and jerk.

- *ridge_factor*: Noise added to the diagonal of the total :moveit_codedir:`quadratic cost matrix<moveit_planners/chomp/chomp_motion_planner/src/chomp_cost.cpp#L62/>` in the objective function. Addition of small noise (e.g., 0.001) allows CHOMP to avoid obstacles at the cost of smoothness in trajectory.

- *use_pseudo_inverse*: Enables pseudo inverse calculations when ``true``.

- *pseudo_inverse_ridge_factor*: Set the ridge factor if pseudo inverse is enabled.

- *joint_update_limit*: Update limit for the robot joints.

- *collision_clearance*: Minimum distance from obstacles needed to avoid collision.

- *collision_threshold*: The cost threshold that that must be maintained to avoid collisions.

- *use_stochastic_descent*: Use stochastic descent while optimizing the cost when set to ``true``. In stochastic descent, a random point from the trajectory is used, rather than all the trajectory points. This is faster and guaranteed to converge, but it may take more iterations in the worst case.

- *enable_failure_recovery*: When ``true``, CHOMP will tweak certain parameters in an attempt to find a solution when one does not exist with the default parameters specified in the ``chomp_planning.yaml`` file.

- *max_recovery_attempts*: Maximum times that CHOMP is run with a varied set of parameters after the first attempt with the default parameters fails.

- *trajectory_initializaiton_method*: The type of trajectory initialization given to CHOMP, which can be ``quintic-spline``, ``linear``, ``cubic`` or ``fillTrajectory``. The first three options refer to the interpolation methods used for trajectory initialization between start and goal states. ``fillTrajectory`` provides an option of initializing the trajectory with a path computed from an existing motion planner like OMPL.

Choosing parameters for CHOMP requires some intuition that is informed by the planning environment. For instance, the default parameters for CHOMP work well in environments without obstacles; however, in environments with many obstacles the default parameters will likely cause CHOMP to get stuck in local minima. By tweaking parameters, we can improve the quality of plans generated by CHOMP.

Some of the unused/commented parameters are *hmc_stochasticity*, *hmc_annealing_factor*, *hmc_discretization*, *use_hamiltonian_montecarlo*, *animate_endeffector*, *animate_endeffector_segment*, *animate_path*, *random_jump_amount*, *add_randomness*.

Difference between plans obtained by CHOMP and OMPL
---------------------------------------------------
Optimizing planners optimize a cost function that may sometimes lead to surprising results: moving through a thin obstacle might be lower cost than a long, winding trajectory that avoids all collisions. In this section we make a distinction between paths obtained from CHOMP and contrast it to those obtained from OMPL.

OMPL is a open source library for sampling based / randomized motion planning algorithms. Sampling based algorithms are probabilistically complete: a solution would be eventually found if one exists, however, non-existence of a solution cannot be reported. These algorithms are efficient and usually find a solution quickly. OMPL does not contain any code related to collision checking or visualization, as the designers of OMPL did not want to tie it to a particular collision checker or visualization front end. The library is designed so it can be easily integrated into systems that provide the additional components. MoveIt integrates directly with OMPL and uses the motion planners from OMPL as its default set of planners. The planners in OMPL are abstract; i.e. OMPL has no concept of a robot. Instead, MoveIt configures OMPL and provides the back-end for OMPL to work with problems in robotics.

CHOMP: While most high-dimensional motion planners separate trajectory generation into distinct planning and optimization stages, CHOMP capitalizes on covariant gradient and functional gradient approaches to the optimization stage to design a motion planning algorithm based entirely on trajectory optimization. Given an infeasible naive trajectory, CHOMP reacts to the surrounding environment to quickly pull the trajectory out of collision while simultaneously optimizing dynamic quantities such as joint velocities and accelerations. It rapidly converges to a smooth, collision-free trajectory that can be executed efficiently on the robot. A covariant update rule ensures that CHOMP quickly converges to a locally optimal trajectory.

For scenes containing obstacles, CHOMP often generates paths which do not prefer smooth trajectories by addition of some noise (*ridge_factor*) in the cost function for the dynamic quantities of the robot (like acceleration, velocity). CHOMP is able to avoid obstacles in most cases, but it can fail if it gets stuck in local minima due to a bad initial guess for the trajectory. OMPL can be used to generate collision-free seed trajectories for CHOMP to mitigate this issue.

Using CHOMP as a post-processor for OMPL
----------------------------------------
Here, we will demonstrate that CHOMP can also be used as a post-processing optimization technique for plans obtained by other planning algorithms. The intuition behind this is that some randomized planning algorithm produces an initial guess for CHOMP. CHOMP then takes this initial guess and further optimizes the trajectory.
To achieve this, use the following steps:

#. Edit ``ompl_planning.yaml`` in the ``<robot_moveit_config>/config`` folder of your robot. Add ``chomp/OptimizerAdapter`` to the bottom of the list of request_adapters: ::

    request_adapters: >-
      ...
      default_planner_request_adapters/FixStartStatePathConstraints
      chomp/OptimizerAdapter

#. Change the ``trajectory_initialization_method`` parameter in ``chomp_planning.yaml`` to ``fillTrajectory`` so that OMPL can provide the input for the CHOMP algorithm: ::

    trajectory_initialization_method: "fillTrajectory"

#. Add the CHOMP config file to the launch file of your robot, ``<robot_moveit_config>/launch/chomp_demo.launch.py``, if it is not there already: ::

    .planning_pipelines(pipelines=["ompl", "chomp"])

#. Now you can launch the newly configured planning pipeline as follows: ::

    ros2 launch moveit2_tutorials chomp_demo.launch.py rviz_tutorial:=True

This will launch RViz. Select OMPL in the Motion Planning panel under the Context tab. Set the desired start and goal states by moving the end-effector around in the same way as was done for CHOMP above. Finally click on the Plan button to start planning. The planner will now first run OMPL, then run CHOMP on OMPL's output to produce an optimized path. To make the planner's task more challenging, add obstacles to the scene using: ::

    ros2 run moveit2_tutorials collision_scene_example
