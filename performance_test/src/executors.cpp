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

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <ostream>
#include <string>

#include "performance_test/executors.hpp"

namespace performance_test
{

std::ostream & operator<<(std::ostream & os, const ExecutorType & t)
{
  std::string executor_name;
  switch (t) {
    case ExecutorType::SINGLE_THREADED_EXECUTOR:
      executor_name = "SingleThreadedExecutor";
      break;
    case ExecutorType::EVENTS_EXECUTOR:
      executor_name = "EventsExecutor";
      break;
    case ExecutorType::MULTI_THREAD_EXECUTOR:
      executor_name = "MultiThreadedExecutor";
      break;
  }

  return os << executor_name;
}

std::shared_ptr<rclcpp::Executor> make_executor(ExecutorType type)
{
  std::shared_ptr<rclcpp::Executor> executor;

  switch (type) {
    case ExecutorType::SINGLE_THREADED_EXECUTOR:
      executor = std::make_shared<rclcpp::executors::SingleThreadedExecutor>();
      break;
    case ExecutorType::EVENTS_EXECUTOR:
      executor = std::make_shared<rclcpp::experimental::executors::EventsExecutor>();
      break;
    case ExecutorType::MULTI_THREAD_EXECUTOR:
      executor = std::make_shared<rclcpp::executors::MultiThreadedExecutor>();
      break;
  }

  return executor;
}

void sleep_task(std::chrono::milliseconds task_duration)
{
  std::mutex mtx;
  std::condition_variable cv;
  bool triggered = false;
  auto ros_context = rclcpp::contexts::get_global_default_context();
  auto callback_handle = ros_context->add_on_shutdown_callback(
    [&]() {
      {
        std::unique_lock<std::mutex> lock(mtx);
        triggered = true;
      }
      cv.notify_all();
    });

  auto wake_up_time = std::chrono::system_clock::now() + task_duration;
  std::unique_lock<std::mutex> lock(mtx);
  cv.wait_until(lock, wake_up_time, [&triggered]() {return triggered;});

  ros_context->remove_on_shutdown_callback(callback_handle);
}

}  // namespace performance_test
