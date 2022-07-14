How to Contribute to This Site
==============================

This explains what you need to know to successfully submit changes to this site.

Learning Objectives
-------------------
- How to locally build this website.
- How to check for broken links.
- How to check spelling and formatting.
- How to run CI locally for checking code changes.

Requirements
------------
- Ubuntu 20.04
- ROS 2 Galactic
- Docker
- A colcon workspace with a copy of `the moveit2_tutorials repo <https://github.com/ros-planning/moveit2_tutorials>`_ (if you don't already have one, :doc:`/doc/tutorials/getting_started/getting_started` will walk you through the process of creating one).

Steps
-----

1. Build and view the website locally

  First, ``cd`` to the root directory of the moveit2_tutorials repo (if you followed the :doc:`Getting Started </doc/tutorials/getting_started/getting_started>` tutorial, this will be ``~/ws_moveit/src/moveit2_tutorials``).  From that directory, run this command:

  .. code-block:: bash

    make html
    # Or run the following lines if you want to automatically rebuild the website on new changes
    while inotifywait -re modify,move,create,delete .; do
      make html
    done

  Then you can open the local build of the site in your web browser from: ``./build/html/index.html``. For example, to open the local site in your default browser the command would be:

  .. code-block:: bash

    xdg-open ./build/html/index.html

2. Test for broken links

  To test for broken links run this command:

  .. code-block:: bash

    ./htmlproofer.sh

3. Run the formatters and spellchecker

  We use `pre-commit <https://pre-commit.com/>`_ to run the formatting and spelling checkers.
  You can install it with pip like this:

  .. code-block:: bash

    python3 -m pip install --user pre-commit

  To run pre-commit locally to fix formatting and spelling:

  .. code-block:: bash

    pre-commit run --all

4. Run industrial_ci locally to run CI

  - Clone a copy of `industrial_ci <https://github.com/ros-industrial/industrial_ci>`_ into your workspace.

  - Build and source your workspace.

  - Run this command from the workspace directory to test code changes just as it is done in CI:

    .. code-block:: bash

      ros2 run industrial_ci rerun_ci src/moveit2_tutorials \
        DOCKER_IMAGE='moveit/moveit2:rolling-source' \
        UPSTREAM_WORKSPACE='moveit2_tutorials.repos' \
        TARGET_CMAKE_ARGS='-DCMAKE_BUILD_TYPE=Release' \
        CCACHE_DIR="$HOME/.ccache" \
        CLANG_TIDY='true'

Further Reading
---------------

- :doc:`how_to_write_tutorials`
- :doc:`how_to_write_how_to_guides`
