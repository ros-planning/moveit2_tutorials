Perception Pipeline Tutorial
============================

MoveIt allows for seamless integration of 3D sensors using `Octomap <http://octomap.github.io/>`_.
Once properly configured, you should see something like this in RViz:

.. image:: perception_configuration_demo.png
   :width: 700px

Getting Started
---------------
If you haven't already done so, make sure you've completed the steps in :doc:`Getting Started </doc/tutorials/getting_started/getting_started>`.

* NOTE-1: Particularly, use ``rolling`` for this tutorial as some dependencies may not be available in ``humble`` or earlier (explained in `moveit2_tutorials!700 <https://github.com/ros-planning/moveit2_tutorials/pull/700#issuecomment-1581411304>`_).
* NOTE-2: As of June 2023, the tutorial code is based on unmerged software changes in the upstream repo. `moveit_resources!181 <https://github.com/ros-planning/moveit_resources/pull/181>`_. Before it's merged, you need to have its source and build by commands e.g. ::

   cd ~/ws_moveit/src
   git clone https://github.com/130s/moveit_resources.git -b feature-panda-moveit-perception
   cd ~/ws_moveit
   sudo apt update && rosdep install -r --from-paths . --ignore-src --rosdistro $ROS_DISTRO -y
   colcon build --mixin release

Configuration
-------------

In this section, we will walk through configuring the 3D sensors on your robot with MoveIt. The primary component in MoveIt that deals with 3D perception is the Occupancy Map Updater. The updater uses a plugin architecture to process different types of input. The currently available plugins in MoveIt are:

* The PointCloud Occupancy Map Updater: which can take as input point clouds (``sensor_msgs/PointCloud2``)

* The Depth Image Occupancy Map Updater: which can take as input Depth Images (``sensor_msgs/Image``)

To use the Occupancy Map Updater, it is only necessary to set the appropriate parameters on the ROS parameter server and to call ``startWorldGeometryMonitor`` from your ``PlanningSceneMonitor``.  This latter step is performed automatically when using Move Group functionality like in this tutorial's example, so in that case it is only necessary to set the parameters for the octomap and octomap updater.

YAML Configuration file (Point Cloud)
+++++++++++++++++++++++++++++++++++++

We will have to generate a YAML configuration file for configuring the 3D sensors. Please see :moveit_resources_codedir:`this example file <panda_moveit_config/config/sensors_3d.yaml>` for processing point clouds.

Save this file in the config folder in the robot's moveit_config package with name "sensors_3d.yaml": ::

  sensors:
    - kinect_pointcloud
  kinect_pointcloud:
      filtered_cloud_topic: filtered_cloud
      max_range: 5.0
      max_update_rate: 1.0
      ns: kinect
      padding_offset: 0.1
      padding_scale: 0.5
      point_cloud_topic: /camera/depth_registered/points
      point_subsample: 1
      sensor_plugin: occupancy_map_monitor/PointCloudOctomapUpdater
  kinect_depthimage:
      far_clipping_plane_distance: 5.0
      filtered_cloud_topic: filtered_cloud
      image_topic: /camera/depth_registered/image_raw
      max_update_rate: 1.0
      near_clipping_plane_distance: 0.3
      ns: kinect
      padding_offset: 0.03
      padding_scale: 4.0
      queue_size: 5
      sensor_plugin: occupancy_map_monitor/DepthImageOctomapUpdater
      shadow_threshold: 0.2
      skip_vertical_pixels: 4
      skip_horizontal_pixels: 6

**The general parameters are:**

* *sensor_plugin*: The name of the plugin that we are using.
* *max_update_rate*: The octomap representation will be updated at rate less than or equal to this value.

**Parameters specific to the Point cloud updater are:**

* *point_cloud_topic*: This specifies the topic to listen on for a point cloud.

* *max_range*: (in m) Points further than this will not be used.

* *point_subsample*: Choose one of every *point_subsample* points.

* *padding_scale*: Should always be >= 1.0. Scale up collision shapes in the scene before excluding them from the octomap.

* *padding_offset*: Absolute padding (in m) around scaled collision shapes when excluding them from the octomap.

* *filtered_cloud_topic*: The topic on which the filtered cloud will be published (mainly for debugging). The filtering cloud is the resultant cloud after self-filtering has been performed.

* *ns*: An optional namespace for the advertised topics. Required for multiple sensors of the same type.

YAML Configuration file (Depth Map)
+++++++++++++++++++++++++++++++++++

We will have to generate a YAML configuration file for configuring the 3D sensors. An :moveit_resources_codedir:`example file for processing depth images <panda_moveit_config/config/sensors_3d.yaml>` can be found in the panda_moveit_config repository as well.
Save this file in the config folder in the robot's moveit_config package with name "sensors_3d.yaml": ::

  sensors:
    - kinect_depthimage
  kinect_depthimage:
      far_clipping_plane_distance: 5.0
      filtered_cloud_topic: filtered_cloud
      image_topic: /camera/depth_registered/image_raw
      max_update_rate: 1.0
      near_clipping_plane_distance: 0.3
      ns: kinect
      padding_offset: 0.03
      padding_scale: 4.0
      queue_size: 5
      sensor_plugin: occupancy_map_monitor/DepthImageOctomapUpdater
      shadow_threshold: 0.2
      skip_vertical_pixels: 4
      skip_horizontal_pixels: 6

**The general parameters are:**

* *sensor_plugin*: The name of the plugin that we are using.
* *max_update_rate*: The octomap representation will be updated at rate less than or equal to this value.

**Parameters specific to the Depth Map updater are:**

* *image_topic*: This specifies the topic to listen on for a depth image.

* *queue_size*: The number of images to queue up.

* *near_clipping_plane_distance*: The minimum distance before lack of visibility.

* *far_clipping_plane_distance*: The maximum distance before lack of visibility.

* *shadow_threshold*: The minimum brightness of the shadow map below an entity for its dynamic shadow to be visible

* *padding_scale*: Should always be >= 1.0. Scale up collision shapes in the scene before excluding them from the octomap.

* *padding_offset*: Absolute padding (in m) around scaled collision shapes when excluding them from the octomap.

* *filtered_cloud_topic*: The topic on which the filtered cloud will be published (mainly for debugging). The filtering cloud is the resultant cloud after self-filtering has been performed.

* *ns*: An optional namespace for the advertised topics. Required for multiple sensors of the same type.

Update the launch file
++++++++++++++++++++++

Add the YAML file to the launch script
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
You will now need to create a *sensor_manager.launch* file in the "launch" directory of your ``panda_moveit_config`` directory with this sensor information, which is already done for you for convenience under :moveit_resources_codedir:`moveit_resources/panda_moveit_config <panda_moveit_config/config/sensor_manager.launch.xml>`.

You will need ``sensors_3d.yaml`` to be read by your application. :moveit_resources_codedir:`In panda_moveit_config/launch/demo.launch.py <panda_moveit_config/launch/demo.launch.py>` the file is already read like the following (It's convoluted, you should use ``demo.launch.py`` from ``moveit_resources`` repo unless you know what this piece of code does.): ::

   from moveit_configs_utils import MoveItConfigsBuilder
   :
   moveit_config = (
        MoveItConfigsBuilder("moveit_resources_panda")
        .robot_description(
            file_path="config/panda.urdf.xacro",
            mappings={
                "ros2_control_hardware_type": LaunchConfiguration(
                    "ros2_control_hardware_type"
                )
            },
        )
        .robot_description_semantic(file_path="config/panda.srdf")
        .trajectory_execution(file_path="config/gripper_moveit_controllers.yaml")
        .planning_pipelines(
            pipelines=["ompl", "chomp", "pilz_industrial_motion_planner", "stomp"])
        .sensors_3d("config/sensors_3d.yaml")
        .to_moveit_configs()
   )

Note that you will need to input the path to the right file you have created above.

Octomap Configuration
^^^^^^^^^^^^^^^^^^^^^
You will also need to configure the `Octomap <http://octomap.github.io/>`_. :moveit_resources_codedir:`In the pre-made example (panda_moveit_config/launch/demo.launch.py) <panda_moveit_config/launch/demo.launch.py>` the following does the job: ::

   def _octomap_launch_params(params: ParameterBuilder):
       params.parameter("octomap_frame", "camera_rgb_optical_frame")
       params.parameter("octomap_resolution", 0.05)
       params.parameter("max_range", 5.0)
       return params.to_dict()
 
MoveIt uses an octree-based framework to represent the world around it. The *Octomap* parameters above are configuration parameters for this representation:
 * *octomap_frame*: specifies the coordinate frame in which this representation will be stored. If you are working with a mobile robot, this frame should be a fixed frame in the world.
 * *octomap_resolution*: specifies the resolution at which this representation is maintained (in meters).
 * *max_range*: specifies the maximum range value to be applied for any sensor input to this node.

Obstacle Avoidance
------------------

If you set the initial and the final location of the robot in a way that there is no straight path between them, then the planner will automatically avoid the octomap and plan around it.

.. image:: obstacle_avoidance.gif
   :width: 700px

Before running the software
+++++++++++++++++++++++++++
This tutorial uses ``moveit2_tutorials`` that depends on ``moveit_task_constructor``, whose installer has not yet been available in ros2 yet (progress tracked in `moveit_task_constructor#400 <https://github.com/ros-planning/moveit_task_constructor/issues/400>`_) so you need to get it via source code. Move into your colcon workspace and pull the MoveIt Task Constructor source: ::

    cd ~/ws_moveit/src
    git clone git@github.com:ros-planning/moveit_task_constructor.git -b ros2
    cd ~/ws_moveit
    colcon build --mixin release
    source ~/ws_moveit/install/setup.bash

Running the Interface
+++++++++++++++++++++

Launch the prepared launch file in moveit_tutorials to see the planning scene integrating sample point cloud data into an octomap: ::

 ros2 launch moveit_tutorials obstacle_avoidance_demo.launch

You should see something like the image shown at the beginning of this tutorial.
If not, you may have run into a `known OpenGL rendering issue <http://wiki.ros.org/rviz/Troubleshooting>`_. To work around the issue, you can force CPU-based rendering with this command:

 export LIBGL_ALWAYS_SOFTWARE=1

You can test obstacle avoidance with the generated octomap for yourself by setting the goal state manually and then planning and executing. To learn how to do that look at `MoveIt Quickstart in RViz </doc/tutorials/quickstart_in_rviz/quickstart_in_rviz_tutorial>`_

Detecting and Adding Object as Collision Object
-----------------------------------------------

In this section, we will demonstrate an example of extracting a cylinder from a pointcloud, computing relevant values and adding it as a collision object to the planning scene.
We will be working with point clouds but it can be implemented similarly with depth maps.

After running the code, you should be able to see something like this in RViz:

.. image:: cylinder_collision_object.png
   :width: 700px

Running the Code
++++++++++++++++

Keep the launch file from above running and run the code directly from moveit_tutorials: ::

  ros2 run moveit2_tutorials detect_and_add_cylinder_collision_object_demo

Relevant Code
+++++++++++++
The entire code can be seen :codedir:`here <examples/perception_pipeline>` in the moveit_tutorials GitHub project.

The details regarding the implementation of each of the perception pipeline function have been omitted in this tutorial as they are well documented on `ros1 wiki <http://wiki.ros.org/pcl/Tutorials>`_.

.. |br| raw:: html

   <br />

.. |code_start| raw:: html

   <code>

.. |code_end| raw:: html

   </code>

.. tutorial-formatter:: ./src/detect_and_add_cylinder_collision_object_demo.cpp
