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

#include "performance_test_factory/factory.hpp"

class TestFactory : public ::testing::Test
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

std::string test_architecture_json_path()
{
  std::string this_file_path = __FILE__;
  std::string this_dir_path = this_file_path.substr(0, this_file_path.rfind("/"));
  return this_dir_path + std::string("/files/test_architecture.json");
}

}  // namespace

// A factory built with callback_group_type=reentrant must propagate it to every
// node it creates from a topology, giving each exactly one Reentrant group.
TEST_F(TestFactory, ReentrantCallbackGroupPassThroughTest)
{
  performance_test_factory::TemplateFactory factory(
    true, true, false, "",
    performance_test_factory::NodeType::RCLCPP_NODE, "reentrant");

  auto nodes_vec = factory.parse_topology_from_json(
    test_architecture_json_path(),
    performance_metrics::Tracker::Options());

  ASSERT_FALSE(nodes_vec.empty());
  for (const auto & node : nodes_vec) {
    EXPECT_EQ(1u, count_reentrant_groups(node));
  }
}

// The default factory leaves every node without a Reentrant group.
TEST_F(TestFactory, DefaultFactoryNoReentrantGroupTest)
{
  performance_test_factory::TemplateFactory factory;

  auto nodes_vec = factory.parse_topology_from_json(
    test_architecture_json_path(),
    performance_metrics::Tracker::Options());

  ASSERT_FALSE(nodes_vec.empty());
  for (const auto & node : nodes_vec) {
    EXPECT_EQ(0u, count_reentrant_groups(node));
  }
}

TEST_F(TestFactory, FactoryConstructorTest)
{
  performance_test_factory::TemplateFactory factory;
}

TEST_F(TestFactory, FactoryCreateFromStringTest)
{
  performance_test_factory::TemplateFactory factory;

  auto node =
    std::make_shared<performance_test::PerformanceNode<rclcpp::Node>>("node_name");

  factory.add_subscriber_from_strings(
    node,
    "stamped10b",
    "my_topic",
    performance_metrics::Tracker::Options());
  factory.add_periodic_publisher_from_strings(
    node,
    "stamped10b",
    "my_topic");
  factory.add_server_from_strings(
    node,
    "stamped10b",
    "my_service");
  factory.add_periodic_client_from_strings(
    node,
    "stamped10b",
    "my_service");

  ASSERT_EQ(1u, node->sub_trackers().size());
  ASSERT_EQ(1u, node->client_trackers().size());
  ASSERT_EQ(1u, node->pub_trackers().size());
}

TEST_F(TestFactory, FactoryCreateFromIndicesTest)
{
  performance_test_factory::TemplateFactory factory;

  int n_subscriber_nodes = 2;
  int n_publisher_nodes = 2;
  std::string msg_type = "stamped10b";
  float frequency = 1;

  int subscriber_start_index = 0;
  int subscriber_end_index = n_subscriber_nodes;
  int publisher_start_index = n_subscriber_nodes;
  int publisher_end_index = n_subscriber_nodes + n_publisher_nodes;

  auto sub_nodes = factory.create_subscriber_nodes(
    subscriber_start_index,
    subscriber_end_index,
    n_publisher_nodes,
    msg_type,
    performance_test::msg_pass_by_t::PASS_BY_SHARED_PTR,
    performance_metrics::Tracker::Options());

  auto pub_nodes = factory.create_periodic_publisher_nodes(
    publisher_start_index,
    publisher_end_index,
    frequency,
    msg_type,
    performance_test::msg_pass_by_t::PASS_BY_UNIQUE_PTR);

  ASSERT_EQ(static_cast<size_t>(2), sub_nodes.size());
  ASSERT_EQ(static_cast<size_t>(2), pub_nodes.size());

  for (const auto & n : sub_nodes) {
    ASSERT_EQ(2u, n->sub_trackers().size());
    ASSERT_EQ(0u, n->client_trackers().size());
    ASSERT_EQ(0u, n->pub_trackers().size());
  }
  for (const auto & n : pub_nodes) {
    ASSERT_EQ(0u, n->sub_trackers().size());
    ASSERT_EQ(0u, n->client_trackers().size());
    ASSERT_EQ(1u, n->pub_trackers().size());
  }
}

TEST_F(TestFactory, FactoryCreateFromJsonTest)
{
  std::string this_file_path = __FILE__;
  std::string this_dir_path = this_file_path.substr(0, this_file_path.rfind("/"));
  std::string json_path = this_dir_path + std::string("/files/test_architecture.json");

  performance_test_factory::TemplateFactory factory;

  auto nodes_vec = factory.parse_topology_from_json(
    json_path,
    performance_metrics::Tracker::Options());

  ASSERT_EQ(static_cast<size_t>(3), nodes_vec.size());

  ASSERT_STREQ("node_0", nodes_vec[0]->get_node_name());
  ASSERT_STREQ("node_1", nodes_vec[1]->get_node_name());
  ASSERT_STREQ("node_2", nodes_vec[2]->get_node_name());

  ASSERT_EQ(0u, nodes_vec[0]->sub_trackers().size());
  ASSERT_EQ(0u, nodes_vec[0]->client_trackers().size());
  ASSERT_EQ(2u, nodes_vec[0]->pub_trackers().size());

  ASSERT_EQ(1u, nodes_vec[1]->sub_trackers().size());
  ASSERT_EQ(0u, nodes_vec[1]->client_trackers().size());
  ASSERT_EQ(0u, nodes_vec[1]->pub_trackers().size());

  ASSERT_EQ(0u, nodes_vec[2]->sub_trackers().size());
  ASSERT_EQ(1u, nodes_vec[2]->client_trackers().size());
  ASSERT_EQ(0u, nodes_vec[2]->pub_trackers().size());
}
