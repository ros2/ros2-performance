// Copyright 2021 iRobot ROS
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

#ifndef PERFORMANCE_TEST__EXECUTORS_HPP_
#define PERFORMANCE_TEST__EXECUTORS_HPP_

#include <chrono>
#include <memory>
#include <ostream>
#include <string>

#include "rclcpp/rclcpp.hpp"
#include "rclcpp/experimental/executors/events_executor/events_executor.hpp"

namespace performance_test
{

struct NamedExecutor
{
  std::shared_ptr<rclcpp::Executor> executor;
  std::string name;
};

enum class ExecutorType
{
  SINGLE_THREADED_EXECUTOR = 1,
  EVENTS_EXECUTOR = 2,
  MULTI_THREAD_EXECUTOR = 3,
  EVENTS_CBG_EXECUTOR = 4,
};

enum class SpinType
{
  SPIN = 1,
  SPIN_SOME = 2,
  SPIN_FUTURE_COMPLETE = 3,
};

std::ostream & operator<<(std::ostream & os, const ExecutorType & t);

// num_threads = 0 means hardware_concurrency for both thread-pool executors
// (MultiThreadedExecutor and EventsCBGExecutor). Ignored for the single-threaded
// executors. Defaulted so existing callers compile unchanged.
std::shared_ptr<rclcpp::Executor> make_executor(ExecutorType type, size_t num_threads = 0);

void sleep_task(std::chrono::milliseconds task_duration);

}  // namespace performance_test

#endif  // PERFORMANCE_TEST__EXECUTORS_HPP_
