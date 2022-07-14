Getting Started
===============

This tutorial will install MoveIt 2 and create a workspace sandbox to run the tutorials and example robot.

Install ROS 2 and Colcon
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
:ros_documentation:`Install ROS 2 {DISTRO_TITLE}<Installation/Ubuntu-Install-Debians.html>`.
It is easy to miss steps when going through the ROS 2 installation tutorial. If you run into errors in the next few steps, a good place to start is to go back and make sure you have installed ROS 2 correctly.  One that users commonly forget is to source the ROS 2 install iself.  ::

  source /opt/ros/foxy/setup.bash

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

Download MoveIt 2 Tutorials Source
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
Move into your colcon workspace and pull the MoveIt 2 tutorials source: ::

<<<<<<< HEAD:doc/getting_started/getting_started.rst
  cd ~/ws_moveit2/src
  git clone https://github.com/ros-planning/moveit2_tutorials.git -b foxy
=======
  cd ~/ws_moveit/src
  git clone https://github.com/ros-planning/moveit2_tutorials -b main

Next we will download the source code for the rest of MoveIt: ::

>>>>>>> 4c408fb (Bulk rename ws_moveit2 --> ws_moveit to conform to docker container directory naming (#411)):doc/tutorials/getting_started/getting_started.rst
  vcs import < moveit2_tutorials/moveit2_tutorials.repos

The import command may ask for your GitHub credentials. You can just press Enter until it moves on (ignore the "Authentication failed" error).

Build your Colcon Workspace
^^^^^^^^^^^^^^^^^^^^^^^^^^^
The following will install from Debian any package dependencies not already in your workspace. This is the step that will install MoveIt and all of its dependencies: ::

  sudo apt update && rosdep install -r --from-paths . --ignore-src --rosdistro $ROS_DISTRO -y

The next command will configure your colcon workspace: ::

  cd ~/ws_moveit
  colcon build --mixin release

Source the colcon workspace: ::

  source ~/ws_moveit/install/setup.bash

Optional: add the previous command to your ``.bashrc``: ::

   echo 'source ~/ws_moveit/install/setup.bash' >> ~/.bashrc

.. note:: Sourcing the ``setup.bash`` automatically in your ``~/.bashrc`` is
   not required and often skipped by advanced users who use more than one
   colcon workspace at a time, but we recommend it for simplicity.

Next Step
^^^^^^^^^^
`Visualize a robot with the interactive motion planning plugin for RViz <../quickstart_in_rviz/quickstart_in_rviz_tutorial.html>`_
