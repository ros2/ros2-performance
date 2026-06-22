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

#ifndef COMPOSITION_BENCHMARK__HELPERS__HELPER_FACTORY_HPP_
#define COMPOSITION_BENCHMARK__HELPERS__HELPER_FACTORY_HPP_

#include <vector>

#include "composition_benchmark/helpers/helper_types.hpp"
#include "performance_test/utils/node_options.hpp"
#include "rclcpp/rclcpp.hpp"

template<typename NodeT>
std::vector<IRobotNodePtr>
create_simple_nodes(int argc, char ** argv)
{
  auto non_ros_args = rclcpp::remove_ros_arguments(argc, argv);

  std::vector<IRobotNodePtr> nodes;
  for (size_t i = 1; i < non_ros_args.size(); i++) {
    std::string node_name = non_ros_args[i];
    auto options = performance_test::create_node_options(node_name);
    IRobotNodePtr node = std::make_shared<NodeT>(options);
    nodes.push_back(node);
  }

  return nodes;
}

#endif  // COMPOSITION_BENCHMARK__HELPERS__HELPER_FACTORY_HPP_
