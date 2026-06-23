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

#include "composition_benchmark/base_node.hpp"
#include "composition_benchmark/helpers/helper_options.hpp"
#include "composition_benchmark/helpers/helper_types.hpp"
#include "performance_metrics/stat_logger.hpp"
#include "performance_test/system.hpp"
#include "performance_test/utils/node_options.hpp"
#include "performance_test_factory/factory.hpp"

static
std::vector<IRobotNodePtr> create_subscriber_nodes(int argc, char ** argv)
{
  auto cli_options = CompositionOptions(argc, argv);
  auto node_options = performance_test::create_node_options(*cli_options.name);

  IRobotNodePtr node = std::make_shared<BaseNode>(node_options);
  const bool ipc = false;
  const bool ros_params = true;
  auto factory = performance_test_factory::TemplateFactory(
    ipc,
    ros_params);
  factory.add_subscriber_from_strings(
    node,
    *cli_options.msg_type,
    "my_topic",
    performance_metrics::Tracker::Options(),
    *cli_options.msg_pass_by);

  return {node};
}

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  auto nodes = create_subscriber_nodes(argc, argv);
  auto system = std::make_unique<performance_test::System>(
    performance_test::ExecutorType::SINGLE_THREADED_EXECUTOR,
    performance_test::SpinType::SPIN);

  system->add_nodes(nodes);
  system->spin(MAX_HOURS, false);

  rclcpp::shutdown();
}
