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

#ifndef COMPOSITION_BENCHMARK__HELPERS__HELPER_OPTIONS_HPP_
#define COMPOSITION_BENCHMARK__HELPERS__HELPER_OPTIONS_HPP_

#include <stdlib.h>

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include "rclcpp/rclcpp.hpp"

struct CompositionOptions
{
  CompositionOptions(int argc, char ** argv)
  {
    std::vector<std::string> args = rclcpp::remove_ros_arguments(argc, argv);

    // All arguments are in pairs + first argument should be program name
    assert((args.size() % 2) != 0);

    for (size_t i = 1; i < args.size(); i += 2) {
      std::string key = args[i];
      std::string str_value = args[i + 1];

      if (key == "subs") {
        num_subs = static_cast<size_t>(atoi(str_value.c_str()));
      } else if (key == "freq") {
        pub_frequency = atoi(str_value.c_str());
      } else if (key == "size") {
        msg_size = atoi(str_value.c_str());
      } else if (key == "msg_type") {
        msg_type = str_value;
      } else if (key == "msg_pass_by") {
        msg_pass_by = performance_test::string_to_msg_pass_by(str_value);
      } else if (key == "ipc") {
        use_ipc = static_cast<bool>(atoi(str_value.c_str()));
      } else if (key == "spin_t") {
        spin_type = str_value;
      } else if (key == "name") {
        name = str_value;
      } else {
        std::cout << "INVALID KEY  " << key << " WITH VALUE " << str_value << std::endl;
        assert(0);
      }
    }
  }

  std::optional<size_t> num_subs {0};
  std::optional<int> pub_frequency {0};
  std::optional<int> msg_size {0};
  std::optional<std::string> msg_type {"stamped_vector"};
  std::optional<performance_test::msg_pass_by_t> msg_pass_by {
    performance_test::msg_pass_by_t::PASS_BY_UNIQUE_PTR};
  std::optional<bool> use_ipc {false};
  std::optional<std::string> spin_type {"spin"};
  std::optional<std::string> name {"node"};
};

#endif  // COMPOSITION_BENCHMARK__HELPERS__HELPER_OPTIONS_HPP_
