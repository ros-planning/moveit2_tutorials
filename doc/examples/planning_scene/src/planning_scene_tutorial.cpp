#include <rclcpp/rclcpp.hpp>

// MoveIt
#include <moveit/robot_model_loader/robot_model_loader.h>
#include <moveit/planning_scene/planning_scene.h>

#include <moveit/kinematic_constraints/utils.h>

// BEGIN_SUB_TUTORIAL stateFeasibilityTestExample
//
// User defined constraints can also be specified to the PlanningScene
// class. This is done by specifying a callback using the
// setStateFeasibilityPredicate function. Here's a simple example of a
// user-defined callback that checks whether the "panda_joint1" of
// the Panda robot is at a positive or negative angle:
bool stateFeasibilityTestExample(const moveit::core::RobotState& robot_state, bool /*verbose*/)
{
  const double* joint_values = robot_state.getJointPositions("panda_joint1");
  return (joint_values[0] > 0.0);
}
// END_SUB_TUTORIAL

static const rclcpp::Logger LOGGER = rclcpp::get_logger("planning_scene_tutorial");

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions node_options;
  node_options.automatically_declare_parameters_from_overrides(true);
  auto planning_scene_tutorial_node = rclcpp::Node::make_shared("planning_scene_tutorial", node_options);

  rclcpp::executors::SingleThreadedExecutor executor;
  executor.add_node(planning_scene_tutorial_node);
  std::thread([&executor]() { executor.spin(); }).detach();

  // BEGIN_TUTORIAL
  //
  // Setup
  // ^^^^^
  //
  // The :moveit_codedir:`PlanningScene<moveit_core/planning_scene/include/moveit/planning_scene/planning_scene.h>`
  // class can be easily setup and configured using a
  // :moveit_codedir:`RobotModel<moveit_core/robot_model/include/moveit/robot_model/robot_model.h>`
  // or a URDF and SRDF. This is, however, not the recommended way to instantiate a
  // PlanningScene. The
  // :moveit_codedir:`PlanningSceneMonitor<moveit_ros/planning/planning_scene_monitor/include/moveit/planning_scene_monitor/planning_scene_monitor.h>`
  // is the recommended method to create and maintain the current
  // planning scene (and is discussed in detail in the next tutorial)
  // using data from the robot's joints and the sensors on the robot. In
  // this tutorial, we will instantiate a PlanningScene class directly,
  // but this method of instantiation is only intended for illustration.

  robot_model_loader::RobotModelLoader robot_model_loader(planning_scene_tutorial_node, "robot_description");
  const moveit::core::RobotModelPtr& kinematic_model = robot_model_loader.getModel();
  planning_scene::PlanningScene planning_scene(kinematic_model);

  // Collision Checking
  // ^^^^^^^^^^^^^^^^^^
  //
  // Self-collision checking
  // ~~~~~~~~~~~~~~~~~~~~~~~
  //
  // The first thing we will do is check whether the robot in its
  // current state is in *self-collision*, i.e. whether the current
  // configuration of the robot would result in the robot's parts
  // hitting each other. To do this, we will construct a
  // :moveit_codedir:`CollisionRequest<moveit_core/collision_detection/include/moveit/collision_detection/collision_common.h>`
  // object and a
  // :moveit_codedir:`CollisionResult<moveit_core/collision_detection/include/moveit/collision_detection/collision_common.h>`
  // object and pass them
  // into the collision checking function. Note that the result of
  // whether the robot is in self-collision or not is contained within
  // the result. Self collision checking uses an *unpadded* version of
  // the robot, i.e. it directly uses the collision meshes provided in
  // the URDF with no extra padding added on.

  collision_detection::CollisionRequest collision_request;
  collision_detection::CollisionResult collision_result;
  planning_scene.checkSelfCollision(collision_request, collision_result);
  RCLCPP_INFO_STREAM(LOGGER, "Test 1: Current state is " << (collision_result.collision ? "in" : "not in")
                                                         << " self collision");
  // Change the state
  // ~~~~~~~~~~~~~~~~
  //
  // Now, let's change the current state of the robot. The planning
  // scene maintains the current state internally. We can get a
  // reference to it and change it and then check for collisions for the
  // new robot configuration. Note in particular that we need to clear
  // the collision_result before making a new collision checking
  // request.

  moveit::core::RobotState& current_state = planning_scene.getCurrentStateNonConst();
  current_state.setToRandomPositions();
  collision_result.clear();
  planning_scene.checkSelfCollision(collision_request, collision_result);
  RCLCPP_INFO_STREAM(LOGGER, "Test 2: Current state is " << (collision_result.collision ? "in" : "not in")
                                                         << " self collision");

  // Checking for a group
  // ~~~~~~~~~~~~~~~~~~~~
  //
  // Now, we will do collision checking only for the hand of the
  // Panda, i.e. we will check whether there are any collisions between
  // the hand and other parts of the body of the robot. We can ask
  // for this specifically by adding the group name "hand" to the
  // collision request.

  collision_request.group_name = "hand";
  current_state.setToRandomPositions();
  collision_result.clear();
  planning_scene.checkSelfCollision(collision_request, collision_result);
  RCLCPP_INFO_STREAM(LOGGER, "Test 3: Current state is " << (collision_result.collision ? "in" : "not in")
                                                         << " self collision");

  // Getting Contact Information
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //
  // First, manually set the Panda arm to a position where we know
  // internal (self) collisions do happen. Note that this state is now
  // actually outside the joint limits of the Panda, which we can also
  // check for directly.

  std::vector<double> joint_values = { 0.0, 0.0, 0.0, -2.9, 0.0, 1.4, 0.0 };
  const moveit::core::JointModelGroup* joint_model_group = current_state.getJointModelGroup("panda_arm");
  current_state.setJointGroupPositions(joint_model_group, joint_values);
  RCLCPP_INFO_STREAM(LOGGER, "Test 4: Current state is "
                                 << (current_state.satisfiesBounds(joint_model_group) ? "valid" : "not valid"));

  // Now, we can get contact information for any collisions that might
  // have happened at a given configuration of the Panda arm. We can ask
  // for contact information by filling in the appropriate field in the
  // collision request and specifying the maximum number of contacts to
  // be returned as a large number.

  collision_request.contacts = true;
  collision_request.max_contacts = 1000;

  //

  collision_result.clear();
  planning_scene.checkSelfCollision(collision_request, collision_result);
  RCLCPP_INFO_STREAM(LOGGER, "Test 5: Current state is " << (collision_result.collision ? "in" : "not in")
                                                         << " self collision");
  collision_detection::CollisionResult::ContactMap::const_iterator it;
  for (it = collision_result.contacts.begin(); it != collision_result.contacts.end(); ++it)
  {
    RCLCPP_INFO(LOGGER, "Contact between: %s and %s", it->first.first.c_str(), it->first.second.c_str());
  }

  // Modifying the Allowed Collision Matrix
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //
  // The
  // :moveit_codedir:`AllowedCollisionMatrix<moveit_core/collision_detection/include/moveit/collision_detection/collision_matrix.h>`
  // (ACM)
  // provides a mechanism to tell the collision world to ignore
  // collisions between certain object: both parts of the robot and
  // objects in the world. We can tell the collision checker to ignore
  // all collisions between the links reported above, i.e. even though
  // the links are actually in collision, the collision checker will
  // ignore those collisions and return not in collision for this
  // particular state of the robot.
  //
  // Note also in this example how we are making copies of both the
  // allowed collision matrix and the current state and passing them in
  // to the collision checking function.

  collision_detection::AllowedCollisionMatrix acm = planning_scene.getAllowedCollisionMatrix();
  moveit::core::RobotState copied_state = planning_scene.getCurrentState();

  collision_detection::CollisionResult::ContactMap::const_iterator it2;
  for (it2 = collision_result.contacts.begin(); it2 != collision_result.contacts.end(); ++it2)
  {
    acm.setEntry(it2->first.first, it2->first.second, true);
  }
  collision_result.clear();
  planning_scene.checkSelfCollision(collision_request, collision_result, copied_state, acm);
  RCLCPP_INFO_STREAM(LOGGER, "Test 6: Current state is " << (collision_result.collision ? "in" : "not in")
                                                         << " self collision");

  // Full Collision Checking
  // ~~~~~~~~~~~~~~~~~~~~~~~
  //
  // While we have been checking for self-collisions, we can use the
  // checkCollision functions instead which will check for both
  // self-collisions and for collisions with the environment (which is
  // currently empty).  This is the set of collision checking
  // functions that you will use most often in a planner. Note that
  // collision checks with the environment will use the padded version
  // of the robot. Padding helps in keeping the robot further away
  // from obstacles in the environment.
  collision_result.clear();
  planning_scene.checkCollision(collision_request, collision_result, copied_state, acm);
  RCLCPP_INFO_STREAM(LOGGER, "Test 7: Current state is " << (collision_result.collision ? "in" : "not in")
                                                         << " self collision");

  // Constraint Checking
  // ^^^^^^^^^^^^^^^^^^^
  //
  // The PlanningScene class also includes easy to use function calls
  // for checking constraints. The constraints can be of two types:
  // (a) constraints chosen from the
  // :moveit_codedir:`KinematicConstraint<moveit_core/kinematic_constraints/include/moveit/kinematic_constraints/kinematic_constraint.h>`
  // set: i.e. JointConstraint, PositionConstraint, OrientationConstraint and
  // VisibilityConstraint and (b) user
  // defined constraints specified through a callback. We will first
  // look at an example with a simple KinematicConstraint.
  //
  // Checking Kinematic Constraints
  // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  //
  // We will first define a simple position and orientation constraint
  // on the end-effector of the panda_arm group of the Panda robot. Note the
  // use of convenience functions for filling up the constraints
  // (these functions are found in the
  // :moveit_codedir:`utils.h<moveit_core/kinematic_constraints/include/moveit/kinematic_constraints/utils.h>`
  // file from the
  // kinematic_constraints directory in moveit_core).

  std::string end_effector_name = joint_model_group->getLinkModelNames().back();

  geometry_msgs::msg::PoseStamped desired_pose;
  desired_pose.pose.orientation.w = 1.0;
  desired_pose.pose.position.x = 0.3;
  desired_pose.pose.position.y = -0.185;
  desired_pose.pose.position.z = 0.5;
  desired_pose.header.frame_id = "panda_link0";
  moveit_msgs::msg::Constraints goal_constraint =
      kinematic_constraints::constructGoalConstraints(end_effector_name, desired_pose);

  // Now, we can check a state against this constraint using the
  // isStateConstrained functions in the PlanningScene class.

  copied_state.setToRandomPositions();
  copied_state.update();
  bool constrained = planning_scene.isStateConstrained(copied_state, goal_constraint);
  RCLCPP_INFO_STREAM(LOGGER, "Test 8: Random state is " << (constrained ? "constrained" : "not constrained"));

  // There's a more efficient way of checking constraints (when you want
  // to check the same constraint over and over again, e.g. inside a
  // planner). We first construct a KinematicConstraintSet which
  // pre-processes the ROS Constraints messages and sets it up for quick
  // processing.

  kinematic_constraints::KinematicConstraintSet kinematic_constraint_set(kinematic_model);
  kinematic_constraint_set.add(goal_constraint, planning_scene.getTransforms());
  bool constrained_2 = planning_scene.isStateConstrained(copied_state, kinematic_constraint_set);
  RCLCPP_INFO_STREAM(LOGGER, "Test 9: Random state is " << (constrained_2 ? "constrained" : "not constrained"));

  // There's a direct way to do this using the KinematicConstraintSet
  // class.

  kinematic_constraints::ConstraintEvaluationResult constraint_eval_result =
      kinematic_constraint_set.decide(copied_state);
  RCLCPP_INFO_STREAM(LOGGER, "Test 10: Random state is "
                                 << (constraint_eval_result.satisfied ? "constrained" : "not constrained"));

  // User-defined constraints
  // ~~~~~~~~~~~~~~~~~~~~~~~~
  //
  // CALL_SUB_TUTORIAL stateFeasibilityTestExample

  // Now, whenever isStateFeasible is called, this user-defined callback
  // will be called.

  planning_scene.setStateFeasibilityPredicate(stateFeasibilityTestExample);
  bool state_feasible = planning_scene.isStateFeasible(copied_state);
  RCLCPP_INFO_STREAM(LOGGER, "Test 11: Random state is " << (state_feasible ? "feasible" : "not feasible"));

  // Whenever isStateValid is called, three checks are conducted: (a)
  // collision checking (b) constraint checking and (c) feasibility
  // checking using the user-defined callback.

  bool state_valid = planning_scene.isStateValid(copied_state, kinematic_constraint_set, "panda_arm");
  RCLCPP_INFO_STREAM(LOGGER, "Test 12: Random state is " << (state_valid ? "valid" : "not valid"));

  // Note that all the planners available through MoveIt and OMPL will
  // currently perform collision checking, constraint checking and
  // feasibility checking using user-defined callbacks.
  // END_TUTORIAL

  rclcpp::shutdown();
  return 0;
}
