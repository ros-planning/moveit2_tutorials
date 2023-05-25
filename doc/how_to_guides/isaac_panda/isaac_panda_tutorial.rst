How To Command Simulated Isaac Robot
====================================

This tutorial requires a machine with ``Isaac Sim 2022.2.0`` installed.
For Isaac Sim requirements and installation please see the `Omniverse documentation <https://docs.omniverse.nvidia.com/app_isaacsim/app_isaacsim/overview.html>`_.

This tutorial has the following assumptions on system configuration:

1. NVIDIA Isaac Sim 2022.2.0 is installed on a Ubuntu 20.04 host in the ``$HOME/.local/share/ov/pkg/isaac_sim-2022.2.0`` directory. (this is the default location)
2. Docker is installed.
3. You clone this repo so that you can build a Ubuntu 22.04 Humble based Docker image that can communicate with Isaac and run this tutorial.

Introduction to ros2_control
----------------------------

One of the recommended ways to execute trajectories calculated by MoveIt is to use the `ros2_control <https://control.ros.org/master/index.html>`_
framework to manage and communicate with your robot, real or simulated. It comes highly recommended because it offers a developers a common API that
allows your software to switch between many different robot types, and the sensors they have built in, by simply changing some launch arguments.
For example if we inspect the Panda Robot's ``ros2_control.xacro`` we can see it uses a flag ``use_fake_hardware`` to switch between being
simulated or connecting to a physical robot.

.. code-block:: XML

    <hardware>
      <xacro:if value="${use_fake_hardware}">
        <plugin>mock_components/GenericSystem</plugin>
      </xacro:if>
      <xacro:unless value="${use_fake_hardware}">
        <plugin>franka_hardware/FrankaHardwareInterface</plugin>
        <param name="robot_ip">${robot_ip}</param>
      </xacro:unless>
    </hardware>


`Hardware Components <https://control.ros.org/master/doc/getting_started/getting_started.html#hardware-components>`_
can be of different types, but the plugin ``<plugin>mock_components/GenericSystem</plugin>`` is very a simple ``System``
that forwards the incoming ``command_interface`` values to the tracked ``state_interface`` of the joints (i.e., perfect control of the simulated joints).

For us to expand our Panda robot to Isaac Sim we first have to introduce `topic_based_ros2_control <https://github.com/PickNikRobotics/topic_based_ros2_control>`_.
This Hardware Interface is a ``System`` that subscribes and publishes on configured topics.
For this tutorial the topic ``/isaac_joint_states`` will contain the robot's current state and ``/isaac_joint_commands`` will be used to actuate it.
The `moveit_resources_panda_moveit_config <https://github.com/ros-planning/moveit_resources/blob/humble/panda_moveit_config/config/panda.ros2_control.xacro#L7>`_
we are using in this tutorial does not support connecting to hardware, so our ``ros2_control.xacro`` is now
updated to load the ``TopicBasedSystem`` plugin when the flag ``ros2_control_hardware_type`` is set to ``isaac``.

.. code-block:: XML

    <xacro:if value="${ros2_control_hardware_type == 'mock_components'}">
        <plugin>mock_components/GenericSystem</plugin>
    </xacro:if>
    <xacro:if value="${ros2_control_hardware_type == 'isaac'}">
        <plugin>topic_based_ros2_control/TopicBasedSystem</plugin>
        <param name="joint_commands_topic">/isaac_joint_commands</param>
        <param name="joint_states_topic">/isaac_joint_states</param>
    </xacro:if>

In this tutorial we have included a Python script that loads a Panda robot
and builds an `OmniGraph <https://docs.omniverse.nvidia.com/prod_extensions/prod_extensions/ext_omnigraph.html>`_
to publish and subscribe to the ROS topics used to control the robot.
To learn about configuring your Isaac Sim robot to communicate with ROS 2 please see the
`Joint Control tutorial <https://docs.omniverse.nvidia.com/app_isaacsim/app_isaacsim/tutorial_ros2_manipulation.html>`_
on Omniverse.

Computer Setup
--------------

1. Install `Isaac Sim <https://docs.omniverse.nvidia.com/app_isaacsim/app_isaacsim/install_workstation.html>`_.

2. Perform a shallow clone of the MoveIt2 Tutorials repo.

.. code-block:: bash

  git clone https://github.com/ros-planning/moveit2_tutorials.git -b main --depth 1

3. Go to the folder in which you cloned the tutorials and then switch to the following directory.

.. code-block:: bash

  cd moveit2_tutorials/doc/how_to_guides/isaac_panda

4. Build the Docker image.

.. code-block:: bash

  docker compose build

Running the MoveIt Interactive Marker Demo with Mock Components
---------------------------------------------------------------

1. To test out the ``mock_components/GenericSystem`` hardware interface run:

.. code-block:: bash

  docker compose up demo_mock_components

This will open up RViz with the Panda robot using ``mock_components`` to simulate the robot and execute trajectories.

Please see the :doc:`Quickstart in RViz </doc/tutorials/quickstart_in_rviz/quickstart_in_rviz_tutorial>`
tutorial if this is your first time using MoveIt with RViz.

After you are done testing press ``Ctrl+C`` in the terminal to stop the container.

Running the MoveIt Interactive Marker Demo with Isaac Sim
---------------------------------------------------------

1. On the host computer, go to the tutorials launch directory.

.. code-block:: bash

  cd moveit2_tutorials/doc/how_to_guides/isaac_panda/launch

2. Then run the following command to load the Panda Robot pre-configured to work with this tutorial.

.. note:: This step assumes Isaac Sim is installed on the host in the ``$HOME/.local/share/ov/pkg/isaac_sim-2022.2.0" directory``.
  This step also takes a few minutes to download the assets and setup Isaac Sim so please be
  patient and don't click the ``Force Quit`` dialog that pops up while the simulator starts.

.. code-block:: bash

  ./python.sh isaac_moveit.py

3. From the ``moveit2_tutorials/doc/how_to_guides/isaac_panda`` directory start a container that connects to Isaac Sim using the ``topic_based_ros2_control/TopicBasedSystem`` hardware interface.

.. code-block:: bash

  docker compose up demo_isaac

This will open up RViz with the Panda robot using the ``TopicBasedSystem`` interface to communicate with the simulated robot and execute trajectories.

.. raw:: html

    <div style="position: relative; padding-bottom: 5%; height: 0; overflow: hidden; max-width: 100%; height: auto;">
        <iframe width="700px" height="400px" src="https://www.youtube.com/embed/af3zkAOWk2Q" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>
    </div>
