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

#ifndef PERFORMANCE_TEST_FACTORY__CLI_OPTIONS_HPP_
#define PERFORMANCE_TEST_FACTORY__CLI_OPTIONS_HPP_

#include <string>
#include <vector>

#include "performance_metrics/tracker.hpp"

namespace performance_test_factory
{

class Options
{
public:
  Options();

  Options(int argc, char ** argv);

  void parse(int argc, char ** argv);

  bool ipc;
  bool ros_params;
  int executor;
  // Number of threads for thread-pool executors (MultiThreadedExecutor,
  // EventsCBGExecutor). 0 means hardware_concurrency.
  int num_threads;
  int node;
  int duration_sec;
  int resources_sampling_per_ms;
  std::vector<std::string> topology_json_list;
  performance_metrics::Tracker::Options tracking_options;
  bool csv_out;
  std::string result_folder_name;
};

std::ostream & operator<<(std::ostream & os, const Options & options);

}  // namespace performance_test_factory

#endif  // PERFORMANCE_TEST_FACTORY__CLI_OPTIONS_HPP_
