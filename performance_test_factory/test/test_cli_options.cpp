// Copyright 2026 iRobot ROS
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

#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "performance_test_factory/cli_options.hpp"

namespace
{

// cxxopts wants a mutable argv; own the strings and hand out char* pointers.
// A dummy topology positional is always included because Options::parse exits
// when no topology file is given, before it validates any other option.
class Argv
{
public:
  explicit Argv(std::vector<std::string> args)
  : storage_(std::move(args))
  {
    for (auto & s : storage_) {
      pointers_.push_back(s.data());
    }
    pointers_.push_back(nullptr);
  }

  int argc() {return static_cast<int>(storage_.size());}
  char ** argv() {return pointers_.data();}

private:
  std::vector<std::string> storage_;
  std::vector<char *> pointers_;
};

}  // namespace

TEST(TestCliOptions, DefaultIsMutuallyExclusive)
{
  Argv args({"irobot_benchmark", "dummy.json"});
  performance_test_factory::Options options;
  options.parse(args.argc(), args.argv());
  EXPECT_EQ("mutually_exclusive", options.callback_group_type);
}

TEST(TestCliOptions, ReentrantIsParsed)
{
  Argv args({"irobot_benchmark", "dummy.json", "--callback-group-type", "reentrant"});
  performance_test_factory::Options options;
  options.parse(args.argc(), args.argv());
  EXPECT_EQ("reentrant", options.callback_group_type);
}

TEST(TestCliOptions, ReentrantIsStreamed)
{
  Argv args({"irobot_benchmark", "dummy.json", "--callback-group-type", "reentrant"});
  performance_test_factory::Options options;
  options.parse(args.argc(), args.argv());

  std::ostringstream os;
  os << options;
  EXPECT_NE(std::string::npos, os.str().find("callback_group_type: reentrant"));
}

// Options::parse catches the cxxopts validation error and calls exit(1), so
// this must run as a death test.
TEST(TestCliOptionsDeathTest, BogusValueExits)
{
  Argv args({"irobot_benchmark", "dummy.json", "--callback-group-type", "bogus"});
  performance_test_factory::Options options;
  EXPECT_EXIT(
    options.parse(args.argc(), args.argv()),
    ::testing::ExitedWithCode(1), "");
}
