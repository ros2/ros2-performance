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

#ifndef PERFORMANCE_TEST__PERFORMANCE_NODE_HPP_
#define PERFORMANCE_TEST__PERFORMANCE_NODE_HPP_

#include <string>

#include "rclcpp/rclcpp.hpp"

#include "performance_test/performance_node_base.hpp"

namespace performance_test
{

template<typename NodeT = rclcpp::Node>
class PerformanceNode : public NodeT, public PerformanceNodeBase
{
public:
  PerformanceNode(
    const std::string & name,
    const std::string & ros2_namespace = "",
    const rclcpp::NodeOptions & node_options = rclcpp::NodeOptions())
  : NodeT(name, ros2_namespace, node_options), PerformanceNodeBase(get_node_interfaces())
  {}

  virtual ~PerformanceNode() = default;

private:
  virtual NodeInterfaces get_node_interfaces()
  {
    return NodeInterfaces {
      this->get_node_base_interface(),
      this->get_node_clock_interface(),
      this->get_node_graph_interface(),
      this->get_node_logging_interface(),
      this->get_node_parameters_interface(),
      this->get_node_services_interface(),
      this->get_node_timers_interface(),
      this->get_node_topics_interface(),
      this->get_node_waitables_interface()
    };
  }
};

}  // namespace performance_test

#endif  // PERFORMANCE_TEST__PERFORMANCE_NODE_HPP_
