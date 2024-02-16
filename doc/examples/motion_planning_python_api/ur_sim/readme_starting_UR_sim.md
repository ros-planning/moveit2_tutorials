# Workspace installation/startup instruction

This installation instruction assumes you have ROS2 installed on a Ubuntu 22 system, and is just a short summary that links the user to relevant other readme files and sometimes has specific instruction for this situation. 
To start create a folder in the home directory for the workspace and go there
   ```
   mkdir -p ~/ros2_ws/src
   cd ~/ros2_ws/src
   ```
In this folder we will clone this repository to make relevant packages for the RMM trials

Source this workspace by adding the following lines in `.bashrc`
```
source ~/ros2_ws/install/setup.bash
source ~/ros2_ws/install/local_setup.bash
```

## Install the Universal robot ROS2 driver
Follow the installation instructions for an installation from source in this workspace on the [Universal Robots ROS2 Driver humble GIT](https://github.com/UniversalRobots/Universal_Robots_ROS2_Driver/tree/humble) 

## Install the Universal robot description package
Follow the installation instructions for an installation from source in this workspace on the [Universal Robots ROS2 Description](https://github.com/UniversalRobots/Universal_Robots_ROS2_Description) 


### Installing Docker for Ubuntu
1. Update the `apt` package index and install packages to allow `apt` to use a repository over HTTPS:

```
sudo apt-get update
sudo apt-get install ca-certificates curl gnupg
```

2. Add Docker’s official GPG key:

```
sudo install -m 0755 -d /etc/apt/keyrings
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo gpg --dearmor -o /etc/apt/keyrings/docker.gpg
sudo chmod a+r /etc/apt/keyrings/docker.gpg
```

3. Use the following command to set up the repository:

```
echo \
  "deb [arch="$(dpkg --print-architecture)" signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
  "$(. /etc/os-release && echo "$VERSION_CODENAME")" stable" | \
  sudo tee /etc/apt/sources.list.d/docker.list > /dev/null
```

4. Update the apt package index:

```
sudo apt-get update
```

5. Install Docker Engine, containerd, and Docker Compose.

```
sudo apt-get install docker-ce docker-ce-cli containerd.io docker-buildx-plugin docker-compose-plugin
```

6. Verify that the Docker Engine installation is successful by running the `hello-world` image.

```
sudo docker run hello-world
```

7. Create the `docker` group.

```
sudo groupadd docker
```

8. Add your user to the `docker` group.

```
sudo usermod -aG docker $USER
```

9. Run the following command to activate the changes to groups:

```
newgrp docker
```

10. Verify that you can run `docker` commands without `sudo`.

```
docker run hello-world
```

## Get the URSIM docker
The CB3 series simulation docker is available installation instruction and more information on how to run can be found [via this website](https://hub.docker.com/r/universalrobots/ursim_cb3)

Simply running the robot simulation can be done using:
```
docker run --rm -it -e ROBOT_MODEL=UR5 -p 5900:5900 -p 6080:6080 universalrobots/ursim_cb3
```

To control the robot via ROS2 (also in simulation) we require to also install and run an external control program on the robot. 

To install and use the URCap external control with a real robot, follow the instructions from the ROS Driver for [e-Series](https://github.com/UniversalRobots/Universal_Robots_ROS_Driver/blob/master/ur_robot_driver/doc/install_urcap_e_series.md)
or for [cb3-Series](https://github.com/UniversalRobots/Universal_Robots_ROS_Driver/blob/master/ur_robot_driver/doc/install_urcap_cb3.md). 
 
For the robot simulation thes steps have been performed already and result in the files that are included in "ursim_init_folder_structure.zip". Place the contents of this .zip in the home directory, such that there is a new folder `.ursim`

The next step is to run the URsim docker and mount the /urcaps folder to a folder containing the urcaps jar files, at start time: 
```
docker run --rm -it -p 5900:5900 -p 6080:6080 -v ${HOME}/.ursim/urcaps:/urcaps -v ${HOME}/.ursim/programs:/ursim/programs --name ursim universalrobots/ursim_cb3
```

With the Simulated robot running and the workspaces sourced it is finally possible to start controlling the robot trough ros
First, launch the robot control which sets up the connection with the simulated robot (This assumes the IP's are al set correctly, the docker takes `172.17.0.2` as default)

```
ros2 launch ur_robot_driver ur_control.launch.py ur_type:=ur5 robot_ip:=172.17.0.2 launch_rviz:=false
```

Using the teachpendant of the simulated robot, start ext_ctrl.urp, to allow the robot to be controlled by a remote interface.

