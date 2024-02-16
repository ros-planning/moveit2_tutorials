#include <rclcpp/rclcpp.hpp>
#include <memory>
// MoveitCpp
#include <moveit/moveit_cpp/moveit_cpp.h>
#include <moveit/moveit_cpp/planning_component.h>

#include <geometry_msgs/msg/point_stamped.h>

#include <moveit_visual_tools/moveit_visual_tools.h>

// Cost functions
#include <moveit/cost_functions/cost_functions.hpp>
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
const rclcpp::Logger LOGGER = rclcpp::get_logger("plannner_cost_fn_example");
const std::string PLANNING_GROUP = "panda_arm";
static const std::vector<std::string> CONTROLLERS(1, "panda_arm_controller");
}  // namespace
namespace plannner_cost_fn_example
{

// Color to string
[[nodiscard]] std::string colorToString(rviz_visual_tools::Colors rviz_color)
{
  std::string color_str = "unknown";
  switch (rviz_color)
  {
    case rviz_visual_tools::Colors::BLACK:
      color_str = "black";
      break;
    case rviz_visual_tools::Colors::BROWN:
      color_str = "brown";
      break;
    case rviz_visual_tools::Colors::BLUE:
      color_str = "blue";
      break;
    case rviz_visual_tools::Colors::CYAN:
      color_str = "cyan";
      break;
    case rviz_visual_tools::Colors::GREY:
      color_str = "grey";
      break;
    case rviz_visual_tools::Colors::DARK_GREY:
      color_str = "dark grey";
      break;
    case rviz_visual_tools::Colors::GREEN:
      color_str = "green";
      break;
    case rviz_visual_tools::Colors::LIME_GREEN:
      color_str = "lime green";
      break;
    case rviz_visual_tools::Colors::MAGENTA:
      color_str = "magenta";
      break;
    case rviz_visual_tools::Colors::ORANGE:
      color_str = "orange";
      break;
    case rviz_visual_tools::Colors::PURPLE:
      color_str = "purple";
      break;
    case rviz_visual_tools::Colors::RED:
      color_str = "red";
      break;
    case rviz_visual_tools::Colors::PINK:
      color_str = "pink";
      break;
    case rviz_visual_tools::Colors::WHITE:
      color_str = "white";
      break;
    case rviz_visual_tools::Colors::YELLOW:
      color_str = "yellow";
      break;
    default:
      break;
  }
  return color_str;
}

/// \brief Utility class to create and interact with the parallel planning demo
class Demo
{
public:
  Demo(rclcpp::Node::SharedPtr node)
    : node_{ node }
    , moveit_cpp_{ std::make_shared<moveit_cpp::MoveItCpp>(node) }
    , planning_component_{ std::make_shared<moveit_cpp::PlanningComponent>(PLANNING_GROUP, moveit_cpp_) }
    , visual_tools_(node, "panda_link0", "plannner_cost_fn_example", moveit_cpp_->getPlanningSceneMonitorNonConst())
  {
    moveit_cpp_->getPlanningSceneMonitorNonConst()->providePlanningSceneService();

    visual_tools_.deleteAllMarkers();
    visual_tools_.loadRemoteControl();

    Eigen::Isometry3d text_pose = Eigen::Isometry3d::Identity();
    text_pose.translation().z() = 1.75;
    visual_tools_.publishText(text_pose, "Cost Function Tutorial", rvt::WHITE, rvt::XLARGE);
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
    for (int index = 0; index < 10; index++)
    {
      moveit_warehouse::MotionPlanRequestWithMetadata planning_query;
      std::string query_name = "kitchen_panda_scene_sensed" + std::to_string(index) + "_query";
      try
      {
        planning_scene_storage->getPlanningQuery(planning_query, scene_name, query_name);
      }
      catch (std::exception& ex)
      {
        RCLCPP_ERROR(LOGGER, "Error loading motion planning query '%s': %s", query_name.c_str(), ex.what());
      }

      motion_plan_requests.push_back(static_cast<moveit_msgs::msg::MotionPlanRequest>(*planning_query));
    }
    visual_tools_.prompt("Press 'next' in the RvizVisualToolsGui window to start the demo");
    visual_tools_.trigger();
    return true;
  }

  struct PipelineConfig
  {
    std::string planning_pipeline;
    std::string planner_id;
    bool use_cost_function;
  };

  /// \brief Request a motion plan based on the assumption that a goal is set and print debug information.
  void planAndVisualize(std::vector<PipelineConfig> pipeline_configs,
                        moveit_msgs::msg::MotionPlanRequest const& motion_plan_request)
  {
    visual_tools_.deleteAllMarkers();
    visual_tools_.prompt("Press 'next' in the RvizVisualToolsGui window to continue the demo");

    // Set goal state
    planning_component_->setGoal(motion_plan_request.goal_constraints);
    // Set start state as current state
    planning_component_->setStartStateToCurrentState();

    // Get start state
    auto robot_start_state = planning_component_->getStartState();

    // Get planning scene
    auto planning_scene_monitor = moveit_cpp_->getPlanningSceneMonitorNonConst();
    planning_scene_monitor->updateFrameTransforms();
    auto planning_scene = [planning_scene_monitor] {
      planning_scene_monitor::LockedPlanningSceneRO ls(planning_scene_monitor);
      return planning_scene::PlanningScene::clone(ls);
    }();

    auto group_name = motion_plan_request.group_name;

    moveit_cpp::PlanningComponent::PlanRequestParameters plan_request_parameters;
    plan_request_parameters.load(node_);
    RCLCPP_INFO_STREAM(
        LOGGER, "Default plan request parameters loaded with --"
                    << " planning_pipeline: " << plan_request_parameters.planning_pipeline << ','
                    << " planner_id: " << plan_request_parameters.planner_id << ','
                    << " planning_time: " << plan_request_parameters.planning_time << ','
                    << " planning_attempts: " << plan_request_parameters.planning_attempts << ','
                    << " max_velocity_scaling_factor: " << plan_request_parameters.max_velocity_scaling_factor << ','
                    << " max_acceleration_scaling_factor: " << plan_request_parameters.max_acceleration_scaling_factor);

    std::vector<planning_interface::MotionPlanResponse> solutions;
    solutions.reserve(pipeline_configs.size());
    for (auto const& pipeline_config : pipeline_configs)
    {
      plan_request_parameters.planning_pipeline = pipeline_config.planning_pipeline;
      plan_request_parameters.planner_id = pipeline_config.planner_id;

      // Set cost function
      if (pipeline_config.use_cost_function)
      {
        planning_component_->setStateCostFunction([robot_start_state, group_name,
                                                   planning_scene](const Eigen::VectorXd& state_vector) mutable {
          auto clearance_cost_fn =
              moveit::cost_functions::createMinJointDisplacementCostFn(*robot_start_state, group_name, planning_scene);
          return clearance_cost_fn(state_vector);
        });
      }
      else
      {
        planning_component_->setStateCostFunction(nullptr);
      }
      auto solution = planning_component_->plan(plan_request_parameters, planning_scene);
      if (pipeline_config.use_cost_function)
      {
        solution.planner_id += std::string("_with_cost_term");
      }

      solutions.push_back(solution);
    }

    int color_index = 1;
    auto robot_model_ptr = moveit_cpp_->getRobotModel();
    auto joint_model_group_ptr = robot_model_ptr->getJointModelGroup(PLANNING_GROUP);
    // Check if PlanningComponents succeeded in finding the plan
    for (auto const& plan_solution : solutions)
    {
      if (plan_solution.trajectory)
      {
        RCLCPP_INFO_STREAM(LOGGER, plan_solution.planner_id.c_str()
                                       << ": " << colorToString(rviz_visual_tools::Colors(color_index))
                                       << ", Path length: " << robot_trajectory::path_length(*plan_solution.trajectory));
        // Visualize the trajectory in Rviz
        visual_tools_.publishTrajectoryLine(plan_solution.trajectory, joint_model_group_ptr,
                                            rviz_visual_tools::Colors(color_index));
        color_index++;
      }
    }
    visual_tools_.trigger();
  }

  moveit_visual_tools::MoveItVisualTools& getVisualTools()
  {
    return visual_tools_;
  }

  std::vector<moveit_msgs::msg::MotionPlanRequest> getMotionPlanRequests()
  {
    return motion_plan_requests;
  }

private:
  std::shared_ptr<rclcpp::Node> node_;
  std::shared_ptr<moveit_cpp::MoveItCpp> moveit_cpp_;
  std::shared_ptr<moveit_cpp::PlanningComponent> planning_component_;
  moveit_visual_tools::MoveItVisualTools visual_tools_;
  std::vector<moveit_msgs::msg::MotionPlanRequest> motion_plan_requests;
};
}  // namespace plannner_cost_fn_example

int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::NodeOptions node_options;
  RCLCPP_INFO(LOGGER, "Initialize node");

  node_options.automatically_declare_parameters_from_overrides(true);
  rclcpp::Node::SharedPtr node = rclcpp::Node::make_shared("plannner_cost_fn_example", "", node_options);

  rclcpp::executors::MultiThreadedExecutor executor;
  executor.add_node(node);
  std::thread([&executor]() { executor.spin(); }).detach();

  plannner_cost_fn_example::Demo demo(node);

  if (!demo.loadPlanningSceneAndQuery())
  {
    rclcpp::shutdown();
    return 0;
  }

  RCLCPP_INFO(LOGGER, "Starting Cost Function Tutorial...");
  for (auto const& motion_plan_req : demo.getMotionPlanRequests())
  {
    demo.planAndVisualize(
        { { "ompl", "RRTConnectkConfigDefault", false }, { "stomp", "stomp", false }, { "stomp", "stomp", true } },
        motion_plan_req);
  }

  demo.getVisualTools().prompt("Press 'next' in the RvizVisualToolsGui window to finish the demo");
  rclcpp::shutdown();
  return 0;
}
