#include <rclcpp/rclcpp.hpp>
#include <memory>
// MoveitCpp
#include <moveit/moveit_cpp/moveit_cpp.h>
#include <moveit/moveit_cpp/planning_component.h>

#include <geometry_msgs/msg/point_stamped.h>

#include <moveit_visual_tools/moveit_visual_tools.h>

// Warehouse
#include <moveit/warehouse/planning_scene_storage.h>
#include <moveit/warehouse/planning_scene_storage.h>
#include <moveit/warehouse/state_storage.h>
#include <moveit/warehouse/constraints_storage.h>
#include <moveit/warehouse/trajectory_constraints_storage.h>
#include <warehouse_ros/database_loader.h>

namespace rvt = rviz_visual_tools;

namespace
{
const rclcpp::Logger LOGGER = rclcpp::get_logger("parallel_planning_example");
const std::string PLANNING_GROUP = "panda_arm";
static const std::vector<std::string> CONTROLLERS(1, "panda_arm_controller");
}  // namespace
namespace parallel_planning_example
{
/// \brief Find shortest trajectory
planning_interface::MotionPlanResponse
getShortestSolution(const std::vector<planning_interface::MotionPlanResponse>& solutions)
{
  // Empty line
  RCLCPP_INFO(LOGGER, "#####################################################");
  RCLCPP_INFO(LOGGER, "###################### Results ######################");
  for (auto const& solution : solutions)
  {
    RCLCPP_INFO(LOGGER, "Planner '%s' returned '%s'", solution.planner_id.c_str(),
                moveit::core::error_code_to_string(solution.error_code).c_str());
    if (solution.trajectory)
    {
      RCLCPP_INFO(LOGGER, "Path length: '%f', Planning time: '%f'", robot_trajectory::path_length(*solution.trajectory),
                  solution.planning_time);
    }
  }
  // Find trajectory with minimal path
  auto const shortest_solution = std::min_element(solutions.begin(), solutions.end(),
                                                  [](const planning_interface::MotionPlanResponse& solution_a,
                                                     const planning_interface::MotionPlanResponse& solution_b) {
                                                    // If both solutions were successful, check which path is shorter
                                                    if (solution_a && solution_b)
                                                    {
                                                      return robot_trajectory::path_length(*solution_a.trajectory) <
                                                             robot_trajectory::path_length(*solution_b.trajectory);
                                                    }
                                                    // If only solution a is successful, return a
                                                    else if (solution_a)
                                                    {
                                                      return true;
                                                    }
                                                    // Else return solution b, either because it is successful or not
                                                    return false;
                                                  });
  RCLCPP_INFO(LOGGER, "'%s' chosen as best solution (Shortest path)", shortest_solution->planner_id.c_str());
  RCLCPP_INFO(LOGGER, "#####################################################");
  return *shortest_solution;
}

/// \brief Utility class to create and interact with the parallel planning demo
class Demo
{
public:
  Demo(rclcpp::Node::SharedPtr node)
    : node_{ node }
    , moveit_cpp_{ std::make_shared<moveit_cpp::MoveItCpp>(node) }
    , planning_component_{ std::make_shared<moveit_cpp::PlanningComponent>(PLANNING_GROUP, moveit_cpp_) }
    , visual_tools_(node, "panda_link0", "parallel_planning_example", moveit_cpp_->getPlanningSceneMonitorNonConst())
  {
    moveit_cpp_->getPlanningSceneMonitorNonConst()->providePlanningSceneService();

    visual_tools_.deleteAllMarkers();
    visual_tools_.loadRemoteControl();

    Eigen::Isometry3d text_pose = Eigen::Isometry3d::Identity();
    text_pose.translation().z() = 1.75;
    visual_tools_.publishText(text_pose, "Parallel Planning Tutorial", rvt::WHITE, rvt::XLARGE);
    visual_tools_.trigger();
  }

  bool loadPlanningSceneAndQuery()
  {
    std::string hostname = "";
    int port = 0.0;
    std::string scene_name = "";

    node_->get_parameter_or(std::string("warehouse.host"), hostname, std::string("127.0.0.1"));
    node_->get_parameter_or(std::string("warehouse.port"), port, 33829);

    if (!node_->get_parameter("warehouse.scene_name", scene_name))
    {
      RCLCPP_WARN(LOGGER, "Warehouse scene_name NOT specified");
    }

    moveit_warehouse::PlanningSceneStorage* planning_scene_storage = nullptr;

    // Initialize database connection
    try
    {
      warehouse_ros::DatabaseLoader db_loader(node_);
      warehouse_ros::DatabaseConnection::Ptr warehouse_connection = db_loader.loadDatabase();
      warehouse_connection->setParams(hostname, port, 20);
      if (warehouse_connection->connect())
      {
        planning_scene_storage = new moveit_warehouse::PlanningSceneStorage(warehouse_connection);
        RCLCPP_INFO(LOGGER, "Connected to database: '%s'", hostname.c_str());
      }
      else
      {
        RCLCPP_ERROR(LOGGER, "Failed to connect to database");
        return false;
      }
    }
    catch (std::exception& e)
    {
      RCLCPP_ERROR(LOGGER, "Failed to initialize planning scene storage: '%s'", e.what());
      return false;
    }

    // Load planning scene
    moveit_msgs::msg::PlanningScene scene_msg;
    try
    {
      if (!planning_scene_storage)
      {
        RCLCPP_ERROR(LOGGER, "No planning scene storage");
        return false;
      }

      if (planning_scene_storage->hasPlanningScene(scene_name))  // Just the world (no robot)
      {
        moveit_msgs::msg::PlanningSceneWorld world_meta_data;
        if (!planning_scene_storage->getPlanningSceneWorld(world_meta_data, scene_name))
        {
          RCLCPP_ERROR(LOGGER, "Failed to load planning scene world '%s'", scene_name.c_str());
          return false;
        }
        scene_msg.world = world_meta_data;
        scene_msg.robot_model_name = "No robot information. Using only world geometry.";
      }
      else
      {
        RCLCPP_ERROR(LOGGER, "Failed to find planning scene '%s'", scene_name.c_str());
        return false;
      }
    }
    catch (std::exception& ex)
    {
      RCLCPP_ERROR(LOGGER, "Error loading planning scene: %s", ex.what());
      return false;
    }

    // Add object to planning scene
    {  // Lock PlanningScene
      planning_scene_monitor::LockedPlanningSceneRW scene(moveit_cpp_->getPlanningSceneMonitorNonConst());
      scene->processPlanningSceneWorldMsg(scene_msg.world);
    }  // Unlock PlanningScene

    RCLCPP_INFO(LOGGER, "Loaded planning scene successfully");

    // Get planning scene query
    moveit_warehouse::MotionPlanRequestWithMetadata planning_query;
    std::string query_name = scene_name + "_query";
    try
    {
      planning_scene_storage->getPlanningQuery(planning_query, scene_name, query_name);
    }
    catch (std::exception& ex)
    {
      RCLCPP_ERROR(LOGGER, "Error loading motion planning query '%s': %s", query_name.c_str(), ex.what());
    }

    planning_query_request_ = static_cast<moveit_msgs::msg::MotionPlanRequest>(*planning_query);
    visual_tools_.prompt("Press 'next' in the RvizVisualToolsGui window to start the demo");
    visual_tools_.trigger();
    return true;
  }

  /// \brief Set joint goal state for next planning attempt
  /// \param [in] panda_jointN Goal value for the Nth joint [rad]
  void setJointGoal(double const panda_joint1, double const panda_joint2, double const panda_joint3,
                    double const panda_joint4, double const panda_joint5, double const panda_joint6,
                    double const panda_joint7)
  {
    auto robot_goal_state = planning_component_->getStartState();
    robot_goal_state->setJointPositions("panda_joint1", &panda_joint1);
    robot_goal_state->setJointPositions("panda_joint2", &panda_joint2);
    robot_goal_state->setJointPositions("panda_joint3", &panda_joint3);
    robot_goal_state->setJointPositions("panda_joint4", &panda_joint4);
    robot_goal_state->setJointPositions("panda_joint5", &panda_joint5);
    robot_goal_state->setJointPositions("panda_joint6", &panda_joint6);
    robot_goal_state->setJointPositions("panda_joint7", &panda_joint7);

    // Set goal state
    planning_component_->setGoal(*robot_goal_state);
  }

  /// \brief Set goal state for next planning attempt based on query loaded from the database
  void setQueryGoal()
  {
    // Set goal state
    if (!planning_query_request_.goal_constraints.empty())
    {
      planning_component_->setGoal(planning_query_request_.goal_constraints);
    }
    else
      RCLCPP_ERROR(LOGGER, "Planning query request does not contain any goal constraints");
  }

  /// \brief Request a motion plan based on the assumption that a goal is set and print debug information.
  void planAndPrint()
  {
    // Set start state as current state
    planning_component_->setStartStateToCurrentState();

    // The MultiPipelinePlanRequestParameters choose a set of planning pipelines to be used for parallel planning. Here,
    // we use all available pipelines but it is also possible to use a subset of pipelines.
    moveit_cpp::PlanningComponent::MultiPipelinePlanRequestParameters multi_pipeline_plan_request{
      node_, { "ompl_rrtc", "pilz_lin", "chomp", "ompl_rrt_star" }
    };

    auto plan_solution = planning_component_->plan(multi_pipeline_plan_request, &getShortestSolution);

    // Check if PlanningComponents succeeded in finding the plan
    if (plan_solution)
    {
      // Visualize the trajectory in Rviz
      auto robot_model_ptr = moveit_cpp_->getRobotModel();
      auto robot_start_state = planning_component_->getStartState();
      auto joint_model_group_ptr = robot_model_ptr->getJointModelGroup(PLANNING_GROUP);

      visual_tools_.publishTrajectoryLine(plan_solution.trajectory, joint_model_group_ptr);
      visual_tools_.trigger();
    }

    // Execute the trajectory and block until it's finished
    moveit_cpp_->execute(plan_solution.trajectory, CONTROLLERS);

    // Start the next plan
    visual_tools_.prompt("Press 'next' in the RvizVisualToolsGui window to continue the demo");
    visual_tools_.deleteAllMarkers();
    visual_tools_.trigger();
  }

private:
  std::shared_ptr<rclcpp::Node> node_;
  std::shared_ptr<moveit_cpp::MoveItCpp> moveit_cpp_;
  std::shared_ptr<moveit_cpp::PlanningComponent> planning_component_;
  moveit_visual_tools::MoveItVisualTools visual_tools_;
  moveit_msgs::msg::MotionPlanRequest planning_query_request_;
};
}  // namespace parallel_planning_example

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions node_options;
  RCLCPP_INFO(LOGGER, "Initialize node");

  node_options.automatically_declare_parameters_from_overrides(true);
  rclcpp::Node::SharedPtr node = rclcpp::Node::make_shared("parallel_planning_example", "", node_options);

  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(node);
  std::thread([&executor]() { executor.spin(); }).detach();

  parallel_planning_example::Demo demo(node);

  if (!demo.loadPlanningSceneAndQuery())
  {
    rclcpp::shutdown();
    return 0;
  }

  RCLCPP_INFO(LOGGER, "Starting MoveIt Tutorials...");

  // Experiment 1 - Short free-space motion, Pilz is expected to create the fastest and shortest solution
  RCLCPP_INFO(LOGGER, "Experiment 1 - Short free-space motion");

  demo.setJointGoal(0.0, -0.8144019900299497, 0.0, -2.6488387075338133, 0.0, 1.8344367175038623, 0.7849999829891612);
  demo.planAndPrint();

  // Experiment 2 - Long motion with collisions, CHOMP and Pilz are likely to fail here due to the difficulty of the planning problem
  RCLCPP_INFO(LOGGER, "Experiment 2 - Long motion with collisions");
  demo.setQueryGoal();
  demo.planAndPrint();

  rclcpp::shutdown();
  return 0;
}
