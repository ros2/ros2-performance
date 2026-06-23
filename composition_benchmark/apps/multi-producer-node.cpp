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

#include "irobot_interfaces_plugin/msg/stamped10b.hpp"
#include "irobot_interfaces_plugin/msg/stamped100kb.hpp"
#include "composition_benchmark/base_node.hpp"
#include "composition_benchmark/helpers/helper_options.hpp"
#include "composition_benchmark/helpers/helper_types.hpp"
#include "performance_test/system.hpp"
#include "performance_test/utils/node_options.hpp"
#include "performance_metrics/resource_usage_logger.hpp"

class MultiProducerNode : public performance_test::PerformanceNode<rclcpp::Node>
{
public:
  explicit MultiProducerNode(const rclcpp::NodeOptions & options = rclcpp::NodeOptions())
  : performance_test::PerformanceNode<rclcpp::Node>(
      "MultiProducerNode",
      "",
      options)
  {
    int topics_number = this->declare_parameter<int>("topics_number", 1);
    std::vector<std::function<void ()>> publish_functions;
    for (int i = 0; i < topics_number; i++) {
      using Msg = irobot_interfaces_plugin::msg::Stamped10b;

      std::string topic_name = "topic_" + std::to_string(i);
      this->add_publisher<Msg>(topic_name, rclcpp::SensorDataQoS());

      auto publish_func = std::bind(
                &MultiProducerNode::publish_msg<Msg>,
                this,
                topic_name,
                performance_test::msg_pass_by_t::PASS_BY_UNIQUE_PTR,
                0,
                std::chrono::milliseconds(10));
      publish_functions.push_back(publish_func);
    }

    this->add_timer(std::chrono::milliseconds(10), [this, publish_functions](){
        for (auto & func : publish_functions) {
          func();
        }
        });

    this->add_periodic_publisher<irobot_interfaces_plugin::msg::Stamped100kb>(
            "slow_topic",
            std::chrono::milliseconds(111),
            performance_test::msg_pass_by_t::PASS_BY_UNIQUE_PTR,
            rclcpp::SensorDataQoS());
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);

  performance_metrics::ResourceUsageLogger ru_logger("/data/log");
  ru_logger.start(std::chrono::milliseconds(500));

  auto nodes = std::make_shared<MultiProducerNode>();
  auto system = std::make_unique<performance_test::System>(
    performance_test::ExecutorType::SINGLE_THREADED_EXECUTOR,
    performance_test::SpinType::SPIN);

  system->add_node(nodes);
  system->spin(std::chrono::seconds(20), false);

  ru_logger.stop();

  rclcpp::shutdown();
}
