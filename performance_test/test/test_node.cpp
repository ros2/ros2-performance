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

#include <chrono>
#include <memory>
#include <string>
#include <vector>

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

namespace
{

size_t count_reentrant_groups(
  const performance_test::PerformanceNodeBase::SharedPtr & node)
{
  size_t count = 0;
  node->get_node_base()->for_each_callback_group(
    [&count](rclcpp::CallbackGroup::SharedPtr group) {
      if (group->type() == rclcpp::CallbackGroupType::Reentrant) {
        count++;
      }
    });
  return count;
}

// Add one of every entity kind so the assertion covers subscription, timer
// (via the periodic publisher), server and client placement.
void add_all_entity_kinds(const performance_test::PerformanceNodeBase::SharedPtr & node)
{
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
}

}  // namespace

// A node built with callback_group_type=reentrant owns exactly one Reentrant
// group, shared by every entity it creates.
TEST_F(TestNode, ReentrantCallbackGroupTest)
{
  rclcpp::NodeOptions node_options;
  node_options.parameter_overrides(
    {{"callback_group_type", std::string("reentrant")}});

  auto node = std::make_shared<performance_test::PerformanceNode<rclcpp::Node>>(
    "node_name", "", node_options);
  add_all_entity_kinds(node);

  EXPECT_EQ(1u, count_reentrant_groups(node));
}

// The default and the explicit mutually_exclusive setting both leave the node
// with no Reentrant group (entities land in the node's default group).
TEST_F(TestNode, DefaultCallbackGroupHasNoReentrantGroupTest)
{
  auto node = std::make_shared<performance_test::PerformanceNode<rclcpp::Node>>("node_name");
  add_all_entity_kinds(node);

  EXPECT_EQ(0u, count_reentrant_groups(node));
}

TEST_F(TestNode, MutuallyExclusiveCallbackGroupTest)
{
  rclcpp::NodeOptions node_options;
  node_options.parameter_overrides(
    {{"callback_group_type", std::string("mutually_exclusive")}});

  auto node = std::make_shared<performance_test::PerformanceNode<rclcpp::Node>>(
    "node_name", "", node_options);
  add_all_entity_kinds(node);

  EXPECT_EQ(0u, count_reentrant_groups(node));
}

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
