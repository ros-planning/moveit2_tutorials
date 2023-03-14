# MoveIt Docker Containers

For more information see the pages [Continuous Integration and Docker](http://moveit.ros.org/documentation/contributing/continuous_integration.html) and [Using Docker Containers with MoveIt](https://moveit.picknik.ai/main/doc/how_to_guides/how_to_setup_docker_containers_in_ubuntu.html).

To build the Docker image locally, run the following from the root folder of this repository.

    docker build -f .docker/Dockerfile -t moveit2_tutorials --build-arg ROS_DISTRO=${ROS_DISTRO} .

where `${ROS_DISTRO}` should be available if you have a ROS installation sourced locally, else you can pick a target release, e.g., `humble` or `rolling`.
