// Copyright 2022 iRobot ROS
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

#include <cstdlib>
#include <string>

#include "composition_benchmark/composable_publisher.hpp"
#include "composition_benchmark/composable_subscriber.hpp"
#include "composition_benchmark/helpers/helper_options.hpp"
#include "composition_benchmark/helpers/helper_types.hpp"
#include "performance_test/system.hpp"
#include "performance_test/utils/node_options.hpp"
#include "rclcpp/rclcpp.hpp"

static
std::vector<IRobotNodePtr> create_pub_sub_system(int argc, char ** argv)
{
  auto options = CompositionOptions(argc, argv);

  bool isolated = false;
  int executor_id = 0;
  if (*options.spin_type == "spin_isolated") {
    isolated = true;
  }

  std::vector<IRobotNodePtr> nodes;

  std::vector<rclcpp::Parameter> pub_parameters = {
    {"executor_id", isolated ? executor_id++ : 0},
    {"topic", "topic"},
    {"frequency", *options.pub_frequency},
    {"size", *options.msg_size}
  };
  auto pub_options = performance_test::create_node_options("pub_node", "", pub_parameters);
  pub_options.use_intra_process_comms(*options.use_ipc);
  IRobotNodePtr pub_node = std::make_shared<ComposablePublisher>(pub_options);
  nodes.push_back(pub_node);

  for (size_t i = 0; i < *options.num_subs; i++) {
    std::string node_name = std::string("sub_node_") + std::to_string(i);
    std::vector<rclcpp::Parameter> sub_parameters = {
      {"executor_id", isolated ? executor_id++ : 0},
      {"topic", "topic"}
    };
    auto sub_options = performance_test::create_node_options(node_name, "", sub_parameters);
    sub_options.use_intra_process_comms(*options.use_ipc);
    IRobotNodePtr sub_node = std::make_shared<ComposableSubscriber>(sub_options);
    nodes.push_back(sub_node);
  }

  return nodes;
}

int main(int argc, char ** argv)
{
  auto options = CompositionOptions(argc, argv);

  performance_test::SpinType spin_type = performance_test::SpinType::SPIN;
  if (*options.spin_type == "spin_some") {
    spin_type = performance_test::SpinType::SPIN_SOME;
  }

  rclcpp::init(argc, argv);

  auto nodes = create_pub_sub_system(argc, argv);
  auto system = std::make_unique<performance_test::System>(
    performance_test::ExecutorType::SINGLE_THREADED_EXECUTOR,
    spin_type);

  system->add_nodes(nodes);
  system->spin(MAX_HOURS, false);

  rclcpp::shutdown();
}
