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

#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "rclcpp_lifecycle/lifecycle_node.hpp"

#include "performance_test/performance_node.hpp"
#include "performance_test_msgs/msg/sample.hpp"
#include "performance_test_msgs/srv/sample.hpp"

class TestNode : public ::testing::Test
{
public:
  static void SetUpTestCase()
  {
    rclcpp::init(0, nullptr);
  }
};

TEST_F(TestNode, NodeConstructorTest)
{
  std::string ros2_namespace = "node_namespace";

  rclcpp::NodeOptions node_options = rclcpp::NodeOptions();
  node_options.use_intra_process_comms(true);
  node_options.start_parameter_services(true);
  node_options.start_parameter_event_publisher(true);

  auto node =
    std::make_shared<performance_test::PerformanceNode<rclcpp::Node>>(
    "node_name",
    ros2_namespace,
    node_options);

  ASSERT_TRUE(node->sub_trackers().empty());
  ASSERT_TRUE(node->client_trackers().empty());
  ASSERT_TRUE(node->pub_trackers().empty());
}

TEST_F(TestNode, NodeAddItemsTest)
{
  auto node = std::make_shared<performance_test::PerformanceNode<rclcpp::Node>>("node_name");

  node->add_subscriber<performance_test_msgs::msg::Sample>(
    "my_topic",
    performance_test::msg_pass_by_t::PASS_BY_SHARED_PTR);
  node->add_periodic_publisher<performance_test_msgs::msg::Sample>(
    "my_topic",
    std::chrono::milliseconds(10),
    performance_test::msg_pass_by_t::PASS_BY_UNIQUE_PTR);
  node->add_server<performance_test_msgs::srv::Sample>(
    "my_service");
  node->add_periodic_client<performance_test_msgs::srv::Sample>(
    "my_service",
    std::chrono::milliseconds(10));

  ASSERT_EQ(1u, node->sub_trackers().size());
  ASSERT_EQ(1u, node->client_trackers().size());
  ASSERT_EQ(1u, node->pub_trackers().size());
}

TEST_F(TestNode, LifecycleNodeConstructorTest)
{
  std::string ros2_namespace = "node_namespace";

  rclcpp::NodeOptions node_options = rclcpp::NodeOptions();
  node_options.use_intra_process_comms(true);
  node_options.start_parameter_services(true);
  node_options.start_parameter_event_publisher(true);

  auto node =
    std::make_shared<performance_test::PerformanceNode<rclcpp_lifecycle::LifecycleNode>>(
    "node_name",
    ros2_namespace,
    node_options);

  ASSERT_TRUE(node->sub_trackers().empty());
  ASSERT_TRUE(node->client_trackers().empty());
  ASSERT_TRUE(node->pub_trackers().empty());
}

TEST_F(TestNode, LifecycleNodeAddItemsTest)
{
  auto node =
    std::make_shared<performance_test::PerformanceNode<rclcpp_lifecycle::LifecycleNode>>(
    "node_name");

  node->add_subscriber<performance_test_msgs::msg::Sample>(
    "my_topic",
    performance_test::msg_pass_by_t::PASS_BY_SHARED_PTR);
  node->add_periodic_publisher<performance_test_msgs::msg::Sample>(
    "my_topic",
    std::chrono::milliseconds(10),
    performance_test::msg_pass_by_t::PASS_BY_UNIQUE_PTR);
  node->add_server<performance_test_msgs::srv::Sample>(
    "my_service");
  node->add_periodic_client<performance_test_msgs::srv::Sample>(
    "my_service",
    std::chrono::milliseconds(10));

  ASSERT_EQ(1u, node->sub_trackers().size());
  ASSERT_EQ(1u, node->client_trackers().size());
  ASSERT_EQ(1u, node->pub_trackers().size());
}
