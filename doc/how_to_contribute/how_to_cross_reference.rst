How to Cross-Reference Content
==============================

This is a primer on how to successfully link to other documents on this website and the API.

There are many and maybe too many different ways to reference content and for new contributors it can be hard to understand what method to use.
Some methods may even work locally but then silently fail to create functional links on the deployed website.
For that reason, we are requesting contributors to only use the suggested Sphinx roles for cross-referencing content on this website.

Learning Objectives
-------------------
* Linking to documents and sections using Sphinx's ``:ref:`` and ``:doc:`` roles
* Using ``:ref:`` ids generated from the *autosectionlabel* extension
* Referencing the C++ API using the ``:cpp_api:`` role from the *doxylink* extension

Linking to other Documents and Sections
---------------------------------------

Sphinx provides the ``:doc:`` and ``:ref:`` roles for `cross-referencing content <https://docs.readthedocs.io/en/stable/guides/cross-referencing-with-sphinx.html#cross-referencing-using-roles>`_ and it's best to stick to them to ensure compatibility with other Sphinx extensions and multi-distro support.

For linking to other documents, you can use the ``:doc:`` role like this: :doc:`/doc/tutorials/getting_started/getting_started` (``:doc:`/doc/tutorials/getting_started/getting_started```). The ``:ref:`` role accepts ids that link to explicit targets on a page.
For convenience, we have enabled the Sphinx extension `autosectionlabel <https://www.sphinx-doc.org/en/master/usage/extensions/autosectionlabel.html>`_ which creates unique and human-readable reference targets for all sections.
Sections in all documents can be linked by providing the document path and the section title :ref:`like this <doc/tutorials/getting_started/getting_started:Install ROS 2 and Colcon>` (``:ref:`like this <doc/tutorials/getting_started/getting_started:Install ROS 2 and Colcon>```).
Note that the ``:doc:`` role requires absolute paths to start with a ``/`` while the *autosectionlabel* extension builds ``:ref`` path labels without it.

Referencing the API Documentation
---------------------------------

The API pages are generated using Doxygen and not Sphinx which means that ``:doc:`` and ``:ref:`` roles are not able to find any API pages.
We are using `doxylink <https://sphinxcontrib-doxylink.readthedocs.io/en/stable/>`_ and the custom ``:cpp_api:`` role for generating links to the API pages from symbols.

Here are some examples, take note that some links use titles and some not:

- namespaces: ``:cpp_api:`moveit::core``` -> :cpp_api:`moveit::core`
- classes:
  ``:cpp_api:`moveit::core::RobotModel``` -> :cpp_api:`moveit::core::RobotModel`
- functions and members:

  - ``:cpp_api:`RobotModel::getName() <moveit::core::RobotModel::getName>``` -> :cpp_api:`RobotModel::getName() <moveit::core::RobotModel::getName>`
  - ``:cpp_api:`moveit::core::RobotModel::enforcePositionBounds(double *state) const``` -> :cpp_api:`moveit::core::RobotModel::enforcePositionBounds(double *state) const`
  - ``:cpp_api:`RobotModel::root_link_ <moveit::core::RobotModel::root_link_>``` -> :cpp_api:`RobotModel::root_link_ <moveit::core::RobotModel::root_link_>`
- files:
  ``:cpp_api:`robot_model.cpp``` -> :cpp_api:`robot_model.cpp`

If you are unsure about how to link certain symbols, you can find all Doxygen references inside the ``MoveIt.tag`` file.
The file is located inside ``build/html/api/`` or ``build/html/<branch>/api/`` depending on the build type.

Do's and Don'ts
---------------

Please **do**:

- Cross-reference as much as possible, especially code
- Provide meaningful titles for links or shorten API symbols to improve readability

Please **don't**:

- Use raw URLs for referencing tutorials or the API
- Link to GitHub source files, prefer the Doxygen pages

Further Reading
---------------

- :doc:`how_to_contribute_to_site`
- :doc:`how_to_write_tutorials`
- :doc:`how_to_write_how_to_guides`
