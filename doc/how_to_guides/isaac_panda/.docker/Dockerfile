FROM osrf/ros:humble-desktop-jammy

SHELL ["/bin/bash", "-c", "-o", "pipefail"]

ARG CUDA_MAJOR_VERSION=11
ARG CUDA_MINOR_VERSION=7

RUN echo "deb [trusted=yes] https://raw.githubusercontent.com/moveit/moveit2_packages/jammy-humble/ ./" \
    | sudo tee /etc/apt/sources.list.d/moveit_moveit2_packages.list
RUN echo "yaml https://raw.githubusercontent.com/moveit/moveit2_packages/jammy-humble/local.yaml humble" \
    | sudo tee /etc/ros/rosdep/sources.list.d/1-moveit_moveit2_packages.list

# Bring the container up to date to get the latest ROS2 humble sync on 01/27/23
# hadolint ignore=DL3008, DL3013
RUN apt-get update && apt-get upgrade -y && rosdep update

# Install packages required to run the demo
RUN apt-get update && apt-get install -y --no-install-recommends \
    ros-humble-moveit \
    ros-humble-moveit-resources \
    wget \
    python3-pip

# Install CUDA
ENV DEBIAN_FRONTEND noninteractive
RUN wget https://developer.download.nvidia.com/compute/cuda/repos/ubuntu2204/x86_64/cuda-keyring_1.0-1_all.deb && \
    dpkg -i cuda-keyring_1.0-1_all.deb && \
    apt-get update && \
    apt-get -y install cuda-cudart-${CUDA_MAJOR_VERSION}-${CUDA_MINOR_VERSION}  cuda-compat-${CUDA_MAJOR_VERSION}-${CUDA_MINOR_VERSION} cuda-toolkit-${CUDA_MAJOR_VERSION}-${CUDA_MINOR_VERSION}
ENV PATH=/usr/local/nvidia/bin:/usr/local/cuda/bin:$PATH
ENV LD_LIBRARY_PATH=/usr/local/nvidia/lib:/usr/local/nvidia/lib64:$LD_LIBRARY_PATH

# Install pytorch
RUN pip3 install torch torchvision

# Create Colcon workspace and clone the needed source code to run the demo
RUN mkdir -p /root/isaac_moveit_tutorial_ws/src
WORKDIR /root/isaac_moveit_tutorial_ws/src
RUN git clone https://github.com/PickNikRobotics/topic_based_ros2_control.git
COPY ./ moveit2_tutorials
WORKDIR /root/isaac_moveit_tutorial_ws
# hadolint ignore=SC1091
RUN source /opt/ros/humble/setup.bash \
    && apt-get update -y \
    && rosdep install --from-paths src --ignore-src --rosdistro "$ROS_DISTRO" -y \
    && rm -rf /var/lib/apt/lists/*

# Use Fast DDS as middleware and load the required config for NVIDIA Isaac Sim.
ENV RMW_IMPLEMENTATION=rmw_fastrtps_cpp
RUN mkdir -p /opt/.ros
COPY ./doc/how_to_guides/isaac_panda/.docker/fastdds.xml /opt/.ros/fastdds.xml
ENV FASTRTPS_DEFAULT_PROFILES_FILE=/opt/.ros/fastdds.xml

ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES compute,utility
ENV NVIDIA_REQUIRE_CUDA "cuda>=${CUDA_MAJOR_VERSION}.${CUDA_MINOR_VERSION}"

# Build the Colcon workspace for the user
RUN source /opt/ros/humble/setup.bash && colcon build

# Set up the entrypoint for both container start and interactive terminals.
COPY ./doc/how_to_guides/isaac_panda/.docker/ros_entrypoint.sh /opt/.ros/
RUN echo "source /opt/.ros/ros_entrypoint.sh" >> ~/.bashrc
ENTRYPOINT [ "/opt/.ros/ros_entrypoint.sh" ]
