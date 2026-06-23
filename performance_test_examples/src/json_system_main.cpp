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

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

#include "performance_metrics/resource_usage_logger.hpp"
#include "performance_metrics/tracker.hpp"
#include "performance_test/system.hpp"
#include "performance_test_factory/factory.hpp"
#include "examples_options.hpp"

using namespace std::chrono_literals;

int main(int argc, char ** argv)
{
  auto options = ExamplesOptions(argc, argv);

  if (options.json_path.empty()) {
    std::string this_file_path = __FILE__;
    std::string this_dir_path = this_file_path.substr(0, this_file_path.rfind("/"));
    options.json_path = this_dir_path + std::string("/simple_architecture.json");
  }

  std::cout << "Json file to load: " << options.json_path << std::endl;
  std::cout << "Intra-process-communication: " << (options.use_ipc ? "on" : "off") << std::endl;
  std::cout << "Run test for: " << options.experiment_duration << " seconds" << std::endl;
  std::cout << "Sampling resources every " << options.resources_sampling_per_ms << "ms" <<
    std::endl;
  std::cout << "Start test" << std::endl;


  std::string create_output_dir_command =
    std::string("mkdir -p ") + options.experiment_path + std::string("/log");
  auto ret = system(create_output_dir_command.c_str());
  static_cast<void>(ret);
  std::string resources_output_path =
    options.experiment_path + std::string("/log/resources.txt");
  std::string events_output_path =
    options.experiment_path + std::string("/log/events.txt");
  std::string latency_all_output_path =
    options.experiment_path + std::string("/log/latency_all.txt");
  std::string latency_total_output_path =
    options.experiment_path + std::string("/log/latency_total.txt");

  // Start resources logger
  performance_metrics::ResourceUsageLogger ru_logger(resources_output_path);
  ru_logger.start(std::chrono::milliseconds(options.resources_sampling_per_ms));

  rclcpp::init(argc, argv);

  // Architecture
  performance_test::System ros2_system(
    static_cast<performance_test::ExecutorType>(options.executor),
    performance_test::SpinType::SPIN,
    events_output_path);

  performance_test_factory::TemplateFactory factory(options.use_ipc);

  auto nodes_vec = factory.parse_topology_from_json(
    options.json_path,
    performance_metrics::Tracker::Options());

  ros2_system.add_nodes(nodes_vec);

  ros2_system.spin(std::chrono::seconds(options.experiment_duration));

  ru_logger.stop();

  rclcpp::shutdown();

  std::this_thread::sleep_for(500ms);

  ros2_system.log_latency_all_stats();
  std::cout << std::endl;
  std::cout << "System total:" << std::endl;
  ros2_system.log_latency_total_stats();
  ros2_system.save_latency_all_stats(latency_all_output_path);
  ros2_system.save_latency_total_stats(latency_total_output_path);

  std::cout << std::endl;
}
