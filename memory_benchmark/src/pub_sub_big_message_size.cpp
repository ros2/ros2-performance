// Copyright 2024 iRobot ROS
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

#include "memory-test.hpp"
#include "memory_benchmark/msg/m0_mb.hpp"
#include "helper_macros.hpp"

#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;

int main(int argc, char * argv[])
{
  print_header();
  print_rss("Start", 0);

  rclcpp::init(argc, argv);
  print_rss("rclcpp::init", 0);

  auto executor = std::make_unique<rclcpp::experimental::executors::EventsExecutor>();
  print_rss("EventsExecutor", 0);

  rclcpp::NodeOptions node_options = rclcpp::NodeOptions();
  node_options.enable_rosout(false);
  node_options.use_intra_process_comms(true);
  node_options.start_parameter_services(false);
  node_options.start_parameter_event_publisher(false);
  // node_options.start_parameter_event_subscriber(false);

  // Profile:
  // * Default Nodes - Params ON (default)            default_nodes.cpp
  // * Nodes - Params OFF                             nodes_params_off.cpp

  // * Default subs - Params OFF                      default_subs_params_off.cpp
  // * Default pubs - Params OFF                      default_pubs_params_off.cpp
  // * Default clients - Params OFF                   default_clients_params_off.cpp
  // * Default services - Params OFF                  default_services_params_off.cpp

  // * Nodes - Logging OFF (check if important)       nodes_params_off_logging off.cpp

  // * Nodes Pub/Subs different topics same msg type  pub_sub_diff_topic.cpp
  // * Nodes Pub/Subs all to same topic               pub_sub_same_topic.cpp
  // * Nodes Pub/Subs different msg types             pub_sub_diff_msg_type.cpp

  // * Nodes Cli/Serv different topics                cli_serv_diff_topics.cpp
  // * Nodes Cli/Servs same topics                    cli_serv_same_topics.cpp

  // * Pub/Sub history depth size big                 pub_sub_big_history_size.cpp
  // * Pub/sub message size big                       pub_sub_big_message_size.cpp

  // RUN_ALL
  // Nodes with a single entity
  // {CREATE_50(NODE)}
  // {CREATE_50(NODE_CLI)}
  // {CREATE_50(NODE_SRV)}
  // {CREATE_50(NODE_CLI_SRV)}
  // {CREATE_50(NODE_CLI_SRV_SAME)}
  // {CREATE_50_TYPED(NODE_P, M1)}
  // {CREATE_50_TYPED(NODE_S, M1)}
  {CREATE_50_TYPED(NODE_PS_DIFF_TOPIC, M0MB)}
  // {CREATE_50_TYPED(NODE_PS_SAME_TOPIC, M1)}
  // {CREATE_50_DIFF_MSG(NODE_PS_DIFF_TOPIC)}

  // One node with multiple entities
  // (we need the node_1 for single entities)
  // {NODE(1) CREATE_50(CLI)}
  // {NODE(1) CREATE_50(SRV)}
  // {NODE(1) CREATE_50_TYPED(SUB,M1)}
  // {NODE(1) CREATE_50_TYPED(PUB,M1)}
  // {NODE(1) CREATE_50_DIFF_MSG(SUB)}
  // {NODE(1) CREATE_50_DIFF_MSG(PUB)}
  // PUBLISH(4, OneMb)

  // std::thread spinner([&](){executor->spin();});spinner.detach();
  // std::this_thread::sleep_for(std::chrono::seconds(2));

  rclcpp::shutdown();
  print_rss("End", 0);
  return 0;
}
