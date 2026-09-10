// Copyright 2019 iRobot ROS
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the iRobot ROS nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#ifndef PERFORMANCE_TEST_FACTORY__FACTORY_HPP_
#define PERFORMANCE_TEST_FACTORY__FACTORY_HPP_

#include <algorithm>
#include <map>
#include <string>
#include <type_traits>
#include <vector>

#include "nlohmann/json.hpp"
#include "performance_test/executors.hpp"
#include "performance_test/performance_node.hpp"
#include "performance_test_factory/node_types.hpp"

namespace performance_test_factory
{

class TemplateFactory
{
public:
  TemplateFactory(
    bool use_ipc = true,
    bool use_ros_params = true,
    bool verbose_mode = false,
    const std::string & ros2_namespace = "",
    NodeType node_type = NodeType::RCLCPP_NODE,
    const std::string & callback_group_type = "mutually_exclusive");

  /**
   * Helper functions for creating several nodes at the same time.
   * These nodes will have names as "node_X", "node_Y"
   * where X, Y, etc, are all the numbers spanning from `start_id` to `end_id`.
   */
  performance_test::PerformanceNodeBase::SharedPtr create_node(
    const std::string & name,
    bool use_ipc = true,
    bool use_ros_params = true,
    bool verbose = false,
    const std::string & ros2_namespace = "",
    int executor_id = 0,
    const std::string & callback_group_type = "mutually_exclusive");

  std::vector<performance_test::PerformanceNodeBase::SharedPtr> create_subscriber_nodes(
    int start_id,
    int end_id,
    int n_publishers,
    const std::string & msg_type,
    performance_test::msg_pass_by_t msg_pass_by,
    const performance_metrics::Tracker::Options & tracking_options,
    const rclcpp::QoS & custom_qos_profile = rclcpp::SensorDataQoS());

  std::vector<performance_test::PerformanceNodeBase::SharedPtr> create_periodic_publisher_nodes(
    int start_id,
    int end_id,
    float frequency,
    const std::string & msg_type,
    performance_test::msg_pass_by_t msg_pass_by,
    size_t msg_size = 0,
    const rclcpp::QoS & custom_qos_profile = rclcpp::SensorDataQoS());

  std::vector<performance_test::PerformanceNodeBase::SharedPtr> create_periodic_client_nodes(
    int start_id,
    int end_id,
    int n_services,
    float frequency,
    const std::string & srv_type,
    const rclcpp::QoS & custom_qos_profile = rclcpp::ServicesQoS());

  std::vector<performance_test::PerformanceNodeBase::SharedPtr> create_server_nodes(
    int start_id,
    int end_id,
    const std::string & srv_type,
    const rclcpp::QoS & custom_qos_profile = rclcpp::ServicesQoS());

  /**
   * Helper functions that, given a node and a std::string describing the msg_type,
   * create the publisher/subscriber/client/server accordingly
   */

  void add_subscriber_from_strings(
    performance_test::PerformanceNodeBase::SharedPtr n,
    const std::string & msg_type,
    const std::string & topic_name,
    const performance_metrics::Tracker::Options & tracking_options,
    performance_test::msg_pass_by_t msg_pass_by =
    performance_test::msg_pass_by_t::PASS_BY_SHARED_PTR,
    const rclcpp::QoS & custom_qos_profile = rclcpp::SensorDataQoS());

  void add_periodic_publisher_from_strings(
    performance_test::PerformanceNodeBase::SharedPtr n,
    const std::string & msg_type,
    const std::string & topic_name,
    performance_test::msg_pass_by_t msg_pass_by =
    performance_test::msg_pass_by_t::PASS_BY_UNIQUE_PTR,
    const rclcpp::QoS & custom_qos_profile = rclcpp::SensorDataQoS(),
    std::chrono::microseconds period = std::chrono::milliseconds(10),
    size_t msg_size = 0);

  void add_periodic_client_from_strings(
    performance_test::PerformanceNodeBase::SharedPtr n,
    const std::string & srv_type,
    const std::string & service_name,
    const rclcpp::QoS & custom_qos_profile = rclcpp::ServicesQoS(),
    std::chrono::microseconds period = std::chrono::milliseconds(10));

  void add_server_from_strings(
    performance_test::PerformanceNodeBase::SharedPtr n,
    const std::string & srv_type,
    const std::string & service_name,
    const rclcpp::QoS & custom_qos_profile = rclcpp::ServicesQoS());

  void add_action_server_from_strings(
    performance_test::PerformanceNodeBase::SharedPtr n,
    const std::string & action_type,
    const std::string & action_name,
    const rclcpp::QoS & custom_qos_profile = rclcpp::ServicesQoS());

  void add_periodic_action_client_from_strings(
    performance_test::PerformanceNodeBase::SharedPtr n,
    const std::string & action_type,
    const std::string & action_name,
    const rclcpp::QoS & custom_qos_profile = rclcpp::ServicesQoS(),
    std::chrono::microseconds period = std::chrono::milliseconds(10));
  /**
   * Helper function that, given a given a json file describing a system,
   * parses it and creates the nodes accordingly
   */

  std::vector<performance_test::PerformanceNodeBase::SharedPtr> parse_topology_from_json(
    const std::string & json_path,
    const performance_metrics::Tracker::Options & tracking_options);

private:
  performance_test::PerformanceNodeBase::SharedPtr create_node_from_json(
    const nlohmann::json & node_json,
    const std::string & suffix = "");

  void create_node_entities_from_json(
    performance_test::PerformanceNodeBase::SharedPtr node,
    const nlohmann::json & node_json,
    const performance_metrics::Tracker::Options & tracking_options);

  void add_periodic_publisher_from_json(
    performance_test::PerformanceNodeBase::SharedPtr node,
    const nlohmann::json & pub_json);

  void add_subscriber_from_json(
    performance_test::PerformanceNodeBase::SharedPtr node,
    const nlohmann::json & sub_json,
    const performance_metrics::Tracker::Options & t_options);

  void add_periodic_client_from_json(
    performance_test::PerformanceNodeBase::SharedPtr node,
    const nlohmann::json & client_json);

  void add_periodic_action_client_from_json(
    performance_test::PerformanceNodeBase::SharedPtr node,
    const nlohmann::json & action_client_json);

  void add_action_server_from_json(
    performance_test::PerformanceNodeBase::SharedPtr node,
    const nlohmann::json & server_json);

  void add_server_from_json(
    performance_test::PerformanceNodeBase::SharedPtr node,
    const nlohmann::json & server_json);

  rclcpp::QoS get_qos_from_json(const nlohmann::json & entity_json);

  performance_test::msg_pass_by_t get_msg_pass_by_from_json(
    const nlohmann::json & entity_json,
    performance_test::msg_pass_by_t default_value);

  bool m_use_ipc;
  bool m_use_ros_params;
  bool m_verbose_mode;
  std::string m_ros2_namespace;
  NodeType m_node_type;
  std::string m_callback_group_type;
};

}  // namespace performance_test_factory

#endif  // PERFORMANCE_TEST_FACTORY__FACTORY_HPP_
