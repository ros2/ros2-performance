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

#include "performance_metrics/stat.hpp"

TEST(StatTest, StatInitTest)
{
  performance_metrics::Stat<uint64_t> stat;

  EXPECT_TRUE(std::isnan(stat.mean()));
  EXPECT_TRUE(std::isnan(stat.stddev()));
  EXPECT_TRUE(std::isnan(stat.min()));
  EXPECT_TRUE(std::isnan(stat.max()));
  ASSERT_EQ((uint64_t)0, stat.n());
}

TEST(StatTest, StatAddSamplesTest)
{
  performance_metrics::Stat<uint64_t> stat;

  // Some samples
  uint64_t s1 = 100000;
  uint64_t s2 = 1000;
  uint64_t s3 = 10000;
  uint64_t s4 = 0;

  stat.add_sample(s1);

  ASSERT_DOUBLE_EQ(static_cast<double>(s1), stat.mean());
  ASSERT_DOUBLE_EQ(static_cast<double>(0), stat.stddev());
  ASSERT_DOUBLE_EQ(static_cast<double>(s1), stat.min());
  ASSERT_DOUBLE_EQ(static_cast<double>(s1), stat.max());

  stat.add_sample(s2);

  ASSERT_DOUBLE_EQ(static_cast<double>(50500), stat.mean());
  ASSERT_DOUBLE_EQ(static_cast<double>(49500), stat.stddev());
  ASSERT_DOUBLE_EQ(static_cast<double>(s2), stat.min());
  ASSERT_DOUBLE_EQ(static_cast<double>(s1), stat.max());

  stat.add_sample(s3);

  ASSERT_DOUBLE_EQ(static_cast<double>(37000), stat.mean());
  EXPECT_NEAR(static_cast<double>(44698.993277254), stat.stddev(), 1e-5);
  ASSERT_DOUBLE_EQ(static_cast<double>(s2), stat.min());
  ASSERT_DOUBLE_EQ(static_cast<double>(s1), stat.max());

  stat.add_sample(s4);

  ASSERT_DOUBLE_EQ(static_cast<double>(27750), stat.mean());
  EXPECT_NEAR(static_cast<double>(41894.95793052), stat.stddev(), 1e-5);
  ASSERT_DOUBLE_EQ(static_cast<double>(s4), stat.min());
  ASSERT_DOUBLE_EQ(static_cast<double>(s1), stat.max());
}
