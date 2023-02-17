=====================
Trajectory Processing
=====================

Time parameterization
---------------------

Motion planners typically only generate “paths”, i.e. there is no timing information associated with the paths.
MoveIt includes several :cpp_api:`trajectory processing <trajectory_processing>` algorithms that can work on these paths and generate trajectories that are properly time-parameterized accounting for the maximum velocity and acceleration limits imposed on individual joints.
These limits are read from a special ``joint_limits.yaml`` configuration file that is specified for each robot.
The configuration file is optional and it overrides any velocity or acceleration limits from the URDF.
The recommended algorithm as of January 2023 is :cpp_api:`TimeOptimalTrajectoryGeneration <trajectory_processing::TimeOptimalTrajectoryGeneration>` (TOTG).
A caveat for this algorithm is that the robot must start and end at rest.
By default, the TOTG timestep is 0.1 seconds.
