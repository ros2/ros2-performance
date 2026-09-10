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

#include <gtest/gtest.h>

#include <thread>

#include "rclcpp/rclcpp.hpp"
#include "performance_test/executors.hpp"

using namespace std::chrono_literals;

TEST(ExecutorsTest, SleepTaskTest)
{
  // This should return immediately
  performance_test::sleep_task(0s);

  // Wait for a small while
  performance_test::sleep_task(10ms);

  bool sleep_done = false;
  std::thread sleeping_thread([&sleep_done]() {
      rclcpp::init(0, nullptr);
      performance_test::sleep_task(1000s);
      sleep_done = true;
    });

  while (!rclcpp::ok()) {
    std::this_thread::sleep_for(1ms);
  }
  ASSERT_TRUE(rclcpp::ok());

  // This should return immediately
  performance_test::sleep_task(0s);

  // Wait for a small while
  performance_test::sleep_task(10ms);

  // Shutdown, this should also wake up the thread
  rclcpp::shutdown();
  sleeping_thread.join();
  EXPECT_TRUE(sleep_done);
}

TEST(ExecutorsTest, MakeExecutorTest)
{
  rclcpp::init(0, nullptr);

  EXPECT_NE(
    nullptr,
    performance_test::make_executor(performance_test::ExecutorType::SINGLE_THREADED_EXECUTOR));
  EXPECT_NE(
    nullptr,
    performance_test::make_executor(performance_test::ExecutorType::EVENTS_EXECUTOR));

  // Thread-pool executors honour num_threads; 0 means hardware_concurrency.
  for (size_t num_threads : {size_t{0}, size_t{1}, size_t{4}}) {
    EXPECT_NE(
      nullptr,
      performance_test::make_executor(
        performance_test::ExecutorType::MULTI_THREAD_EXECUTOR, num_threads));
    EXPECT_NE(
      nullptr,
      performance_test::make_executor(
        performance_test::ExecutorType::EVENTS_CBG_EXECUTOR, num_threads));
  }

  rclcpp::shutdown();
}
