# MoveIt Tutorials
[Live tutorials here](https://moveit.picknik.ai/)

This is the primary documentation for the MoveIt project.
## Build Status

This repository is built and deployed automatically by GitHub Actions:

- **Rolling** (main): [![CI](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/ci.yaml/badge.svg?branch=main)](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/ci.yaml?query=branch%3Amain) [![Format](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/format.yml/badge.svg?branch=main)](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/format.yml?query=branch%3Amain) [![Deploy](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/deploy.yml/badge.svg?branch=main)](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/deploy.yml?query=branch%3Amain)
- **Humble**: [![CI](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/ci.yaml/badge.svg?branch=humble)](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/ci.yaml?query=branch%3Ahumble) [![Format](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/format.yml/badge.svg?branch=humble)](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/format.yml?query=branch%3Ahumble) [![Deploy](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/deploy.yml/badge.svg?branch=humble)](https://github.com/ros-planning/moveit2_tutorials/actions/workflows/deploy.yml?query=branch%3Ahumble)

## Contributing

We strongly encourage you to help improve MoveIt's documentation. Please consider helping improve the tutorials, port old ones from ROS 1, and write new tutorials. We recommend you  read the quality standards below as well as the [How to Write a MoveIt Tutorial](https://moveit.picknik.ai/main/doc/how_to_contribute/how_to_write_tutorials.html) page.

If you find an issue with the tutorials you are not able to fix yourself, please [open an issue on GitHub](https://github.com/ros-planning/moveit2_tutorials/issues/new) or open a PR with proposed changes.

## Helping with Porting Tutorials to ROS 2

An issue has been created for each tutorial to be ported to ROS 2. At the top of each tutorial, there is a tag: ":moveit1:", remove the tag
after the tutorial has been successfully updated.

Below are some links to help with the ports.

* [colcon](https://colcon.readthedocs.io/en/released/user/how-to.html)
* [ament](https://index.ros.org/doc/ros2/Tutorials/Ament-CMake-Documentation/)
* [rclcpp](https://docs.ros2.org/latest/api/rclcpp/index.html)


## MoveIt Tutorials Source Build

Follow the [MoveIt Source Build](https://moveit.ros.org/install-moveit2/source/) instructions to set up a colcon workspace with MoveIt from the source.

Open a command line to your colcon workspace:

    cd $COLCON_WS/src

Download the MoveIt Tutorials source code:

    git clone https://github.com/ros-planning/moveit2_tutorials.git
    vcs import < moveit2_tutorials/moveit2_tutorials.repos
    rosdep install -r --from-paths . --ignore-src --rosdistro rolling -y

Configure and build the workspace:

    cd $COLCON_WS
    colcon build --event-handlers desktop_notification- status- --cmake-args -DCMAKE_BUILD_TYPE=Release

## Build HTML Pages Locally

If you want to test the tutorials by generating the HTML pages locally on your machine, you can use the ``build_locally`` script by issuing the following commands in the root of the moveit2_tutorials package:

    export ROS_DISTRO=rolling  # 20.04

    cd $COLCON_WS/src/moveit2_tutorials
    source /opt/ros/$ROS_DISTRO/setup.bash
    ./build_locally.sh

The local website ``<LOCAL_PACKAGE_PATH>/build/html/index.html`` should automatically open in your web browser.

### Optional build_locally Settings

 - *noinstall* skip the dependencies install step to speed up the script
 - *loop* automatically rebuilds the HTML if a change is detected

### Formatting and Style

These tutorials use the [reStructuredText](http://www.sphinx-doc.org/en/stable/rest.html) format commonly used in the Sphinx "Python Documentation Generator". This unfortunately differs from the common Markdown format, but its advantage is that it supports embedding code directly from source files for inline code tutorials.

**Code Formatting**

* These tutorials use the same [style guidelines](http://moveit.ros.org/documentation/contributing/code/) as the MoveIt project. When modifying or adding to these tutorials, it is required that code is auto-formatted using [clang format](http://moveit.ros.org/documentation/contributing/code/). To check and apply the style guidelines we use [pre-commit](https://pre-commit.com/).
* Tutorials should exemplify best coding practices. If a contribution wouldn't pass review in the MoveIt project, then it shouldn't pass review in the tutorials.
* Relevant code should be included and explained using the ``.. tutorial-formatter::`` tag.
* Irrelevant code should be excluded from the generated HTML using the ``BEGIN_TUTORIAL``, ``END_TUTORIAL``, ``BEGIN_SUB_TUTORIAL``, and ``END_SUB_TUTORIAL`` tags.
* Whenever possible, links should be created using the ``extlinks`` dictionary defined in ``conf.py``.
* All demo code should be runnable from within the ``moveit2_tutorials`` package.
* Python code should be run using ``ros2 run``.

**Style**

* Each tutorial should be focused on teaching the user one feature or interface within MoveIt.
* Tutorials should flow from show to tell with videos and demos at the beginning followed by explanations.
* New tutorials should match the formatting, style, and flow of existing tutorials whenever possible.

**pre-commit**

pre-commit is a tool that is used in moveit2_tutorials to check and apply style guidelines automatically. To install pre-commit into your system:

    pip3 install pre-commit

Then under the moveit2_tutorials directory install the git hooks like this:

    cd $COLCON_WS/src/moveit2_tutorials && pre-commit install

With this pre-commit will automatically run and check a list of styling including clang-format, end of files, and trailing whitespaces whenever you run `git commit`. To run pre-commit any time other than `git commit`:

    cd $COLCON_WS/src/moveit2_tutorials && pre-commit run -a

### Including Images and Videos
#### Images
The standard way to include an image in reStructuredText is
```
.. image:: filename.png
   :width: 700px
```

This assumes that `filename.png` is in the same folder as the source `.rst` file. Images linked in this way will automatically be copied to the appropriate folder in the build.

[External Documentation](https://sublime-and-sphinx-guide.readthedocs.io/en/latest/images.html)

Do **not** include animated gifs as the file format leads to very large files. Use a video format like `webm` and see the section on the local video below.

#### YouTube and other External Video
You can embed video with raw HTML, like in this example from the Pick and Place Tutorial.
```
.. raw:: html

    <div style="position: relative; padding-bottom: 5%; height: 0; overflow: hidden; max-width: 100%; height: auto;">
        <iframe width="700px" height="400px" src="https://www.youtube.com/embed/QBJPxx_63Bs?rel=0" frameborder="0" allow="autoplay; encrypted-media" allowfullscreen></iframe>
    </div>
```
This includes [Youtube's suggested embed HTML](https://support.google.com/youtube/answer/171780?hl=en).

#### Local Video
To embed a video that is included in this repository, you also will use raw HTML, like this example from the Quickstart in RViz tutorial.

```
.. raw:: html

    <video width="700px" controls="true" autoplay="true" loop="true">
        <source src="../../../_static/videos/rviz_joints_nullspace.webm" type="video/webm">
        The joints move while the end effector stays still
    </video>
```

Note that the video file is in the `_static/videos` folder instead of the same folder.

[External Documentation on &lt;video&gt; tag](https://developer.mozilla.org/en-US/docs/Web/HTML/Element/video)

## License

All content in this repository is open source and released under the [BSD License v3](https://opensource.org/licenses/BSD-3-Clause). Each source code file should contain a copy of the license.
