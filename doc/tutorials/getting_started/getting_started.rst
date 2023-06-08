Getting Started
===============

Here, we will setup your environment for best running the tutorials.
This will create a colcon workspace, download all of the latest MoveIt source code, and build everything from source to ensure you have the latest fixes and improvements.

Building all the source code of MoveIt can take 20-30 minutes, depending on the CPU speed and available RAM of your computer.
If you are on a less performant system, or generally just want to get started quicker, check out our :doc:`Docker Guide </doc/how_to_guides/how_to_setup_docker_containers_in_ubuntu>`.

Install ROS 2 and colcon
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
MoveIt 2 currently supports multiple versions of ROS.
Install whichever version you prefer.
We primarily support ROS installed Ubuntu 22.04 but other methods and platforms may work with small changes to the instructions listed below.
If you are just getting started, we recommend you use the latest stable version of ROS (Iron) on Ubuntu 22.04 for the most seamless experience.

* `Rolling Ridley <https://docs.ros.org/en/rolling/Installation.html>`_ - Rolling Development Release
* `Iron Irwini <https://docs.ros.org/en/iron/Installation.html>`_ - Latest Stable Release - May 2023
* `Humble Hawksbill <https://docs.ros.org/en/humble/Installation.html>`_ - LTS Release - May 2022

It is easy to miss steps when going through the ROS 2 installation tutorial.
If you run into errors in the next few steps, a good place to start is to go back and make sure you have installed ROS 2 correctly.
One that users commonly forget is to source the ROS 2 install itself.
Note to source the version of ROS you installed.  ::

  source /opt/ros/iron/setup.bash

.. note:: Unlike ROS 1 setup scripts, in ROS 2 the setup scripts do not attempt to switch what version of ROS you are using.  This means that if you have previously sourced a different version of ROS, including from within your ``.bashrc`` file, you will run into errors during the building step.  To fix this change what is sourced in your ``.bashrc`` and start a new terminal.

Install `rosdep <http://wiki.ros.org/rosdep>`_ to install system dependencies : ::

  sudo apt install python3-rosdep

Once you have ROS 2 installed, make sure you have the most up to date packages: ::

  sudo rosdep init
  rosdep update
  sudo apt update
  sudo apt dist-upgrade

Install :ros_documentation:`Colcon <Tutorials/Colcon-Tutorial.html#install-colcon>` the ROS 2 build system with `mixin <https://github.com/colcon/colcon-mixin-repository>`_: ::

  sudo apt install python3-colcon-common-extensions
  sudo apt install python3-colcon-mixin
  colcon mixin add default https://raw.githubusercontent.com/colcon/colcon-mixin-repository/master/index.yaml
  colcon mixin update default

Install `vcstool <https://index.ros.org/d/python3-vcstool/>`_ : ::

  sudo apt install python3-vcstool

Create A Colcon Workspace and Download Tutorials
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
For tutorials you will need to have a :ros_documentation:`colcon <Tutorials/Colcon-Tutorial.html#install-colcon>` workspace setup. ::

  mkdir -p ~/ws_moveit/src

Download Source Code of MoveIt and the Tutorials
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Move into your Colcon workspace and pull the MoveIt tutorials source: ::

  cd ~/ws_moveit/src
  git clone https://github.com/ros-planning/moveit2_tutorials -b main --depth 1

Next we will download the source code for the rest of MoveIt: ::

  vcs import < moveit2_tutorials/moveit2_tutorials.repos

The import command may ask for your GitHub credentials.
You can just press Enter until it moves on (ignore the "Authentication failed" error).

Build your Colcon Workspace
^^^^^^^^^^^^^^^^^^^^^^^^^^^
The following will install from Debian any package dependencies not already in your workspace.
This is the step that will install MoveIt and all of its dependencies: ::

  sudo apt update && rosdep install -r --from-paths . --ignore-src --rosdistro $ROS_DISTRO -y

The next command will configure your Colcon workspace: ::

  cd ~/ws_moveit
  colcon build --mixin release


This build command will likely take a long time (20+ minutes) depending on your computer speed and amount of RAM available (we recommend 32 GB).

.. warning::
  Some of the packages built with this command require up to 16Gb of RAM to build. By default, ``colcon``  tries to build as many packages as possible at the same time.
  If you are low on computer memory, or if the build is generally having trouble completing on your computer,
  you can try appending ``--executor sequential`` to the ``colcon`` command above to build only one package at a time, or ``-parallel-workers <X>`` to limit the number of simultaneous builds.

If everything goes well, you should see the message "Summary: X packages finished" where X might be 50. If you have problems, try re-checking your `ROS Installation <https://docs.ros.org/en/rolling/Installation.html>`_.

Setup Your Colcon Workspace
^^^^^^^^^^^^^^^^^^^^^^^^^^^

Source the Colcon workspace: ::

  source ~/ws_moveit/install/setup.bash

Optional: add the previous command to your ``.bashrc``: ::

   echo 'source ~/ws_moveit/install/setup.bash' >> ~/.bashrc

.. note:: Sourcing the ``setup.bash`` automatically in your ``~/.bashrc`` is
   not required and often skipped by advanced users who use more than one
   Colcon workspace at a time, but we recommend it for simplicity.

Switch to Cyclone DDS
^^^^^^^^^^^^^^^^^^^^^

As of Sep 26, 2022, the default ROS 2 middleware (RMW) implementation has an issue.
As a workaround, switch to Cyclone DDS.
(Note: this makes all nodes started using this RMW incompatible with any other nodes not using Cyclone DDS.) ::

  sudo apt install ros-rolling-rmw-cyclonedds-cpp
  # You may want to add this to ~/.bashrc to source it automatically
  export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp

Next Step
^^^^^^^^^
Nice job!
Next, we will :doc:`Visualize a robot with the interactive motion planning plugin for RViz </doc/tutorials/quickstart_in_rviz/quickstart_in_rviz_tutorial>`
