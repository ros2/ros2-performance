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

#include <chrono>

#include "performance_metrics/dummy_work.hpp"

TEST(DummyWork, DummyWorkTest)
{
  using namespace std::chrono_literals;

  auto t1 = std::chrono::high_resolution_clock::now();
  performance_metrics::dummy_work(0us);

  auto t2 = std::chrono::high_resolution_clock::now();
  performance_metrics::dummy_work(5ms);

  auto t3 = std::chrono::high_resolution_clock::now();
  performance_metrics::dummy_work(100ms);

  auto t4 = std::chrono::high_resolution_clock::now();

  auto first_run_duration = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1);
  auto second_run_duration = std::chrono::duration_cast<std::chrono::microseconds>(t3 - t2);
  auto third_run_duration = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3);

  EXPECT_GT(second_run_duration.count(), first_run_duration.count());
  EXPECT_GT(third_run_duration.count(), second_run_duration.count());
}
