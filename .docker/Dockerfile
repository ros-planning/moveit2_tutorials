# syntax = docker/dockerfile:1.3

ARG ROS_DISTRO=rolling

######################### Tutorial Image  #################################################

FROM moveit/moveit2:${ROS_DISTRO}-source as tutorial_image

LABEL org.opencontainers.image.description "This container has working versions of the tutorials discussed here: https://moveit.picknik.ai/main/doc/tutorials/tutorials.html"

# Copy MoveIt sources from docker context
COPY . src/moveit2_tutorials

# Commands are combined in single RUN statement with "apt/lists" folder removal to reduce image size
# https://docs.docker.com/develop/develop-images/dockerfile_best-practices/#minimize-the-number-of-layers
RUN --mount=type=cache,target=/root/.ccache/ \
    # Fetch required upstream sources for building
    vcs import src < /root/ws_moveit/src/moveit2_tutorials/.github/upstream.repos

######################### Hello World Tutorial  #########################################

# Create a new package using the command from the tutorial
RUN . "/opt/ros/${ROS_DISTRO}/setup.sh" &&\
    . "install/setup.sh" &&\
    cd "src" &&\
    ros2 pkg create \
    --build-type ament_cmake \
    --dependencies moveit_ros_planning_interface moveit_visual_tools rclcpp \
    --node-name hello_moveit hello_moveit

######################### Planning Around Objects  #######################################

# Remove the hello_world tutorial cpp file and replace it with the planning_around_objects file
RUN rm src/hello_moveit/src/hello_moveit.cpp
COPY ./doc/tutorials/planning_around_objects/hello_moveit_kinova.cpp src/hello_moveit/src/hello_moveit.cpp

######################### Pick and Place (MTC) Image  #########################################

# Create a new package using the command from the tutorial
RUN . "/opt/ros/${ROS_DISTRO}/setup.sh" &&\
    . "install/setup.sh" &&\
    ros2 pkg create \
        --build-type ament_cmake \
        --destination-directory src \
        --dependencies moveit_task_constructor_core rclcpp \
        --node-name mtc_node mtc_tutorial

# Remove the empty cpp file and replace it with the example file
RUN rm src/mtc_tutorial/src/mtc_node.cpp
COPY ./doc/tutorials/pick_and_place_with_moveit_task_constructor/src/mtc_node.cpp src/mtc_tutorial/src/mtc_node.cpp

# Add the launch folder to the tutorial package and CMakeLists.txt
COPY ./doc/tutorials/pick_and_place_with_moveit_task_constructor/launch src/mtc_tutorial/launch

# Add install(DIRECTORY launch  DESTINATION share/${PROJECT_NAME}) to CMakeLists.txt
RUN sed -i "s|ament_package()|install(DIRECTORY launch  DESTINATION share/\${PROJECT_NAME})\nament_package()|g" src/mtc_tutorial/CMakeLists.txt
# Build the tutorials and set up the entrypoint/bashrc
RUN --mount=type=cache,target=/root/.ccache/ \
    # Enable ccache
    . "/opt/ros/${ROS_DISTRO}/setup.sh" &&\
     . "install/setup.sh" &&\
    sudo apt update && rosdep install -r --from-paths src --ignore-src --rosdistro $ROS_DISTRO -y && \
    sudo apt install -y ros-${ROS_DISTRO}-rmw-cyclonedds-cpp && \
    colcon build \
        --cmake-args -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=OFF -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
                     -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
        --ament-cmake-args -DCMAKE_BUILD_TYPE=Release \
        --event-handlers desktop_notification- status- && \
    ccache -s && \
    rm -rf /var/lib/apt/lists/* && \
    # Update /ros_entrypoint.sh to source our new workspace
    sed -i "s#/opt/ros/\$ROS_DISTRO/setup.bash#$ROS_UNDERLAY/install/setup.bash#g" /ros_entrypoint.sh && \
    echo "export RMW_IMPLEMENTATION=rmw_cyclonedds_cpp" >> /root/.bashrc && \
    echo "source /opt/ros/${ROS_DISTRO}/setup.bash" >> /root/.bashrc && \
    echo "source /root/ws_moveit/install/setup.bash" >> /root/.bashrc
