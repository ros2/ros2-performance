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

#include <memory>
#include <string>

#include "performance_test_factory/load_plugins.hpp"

namespace performance_test_factory
{

static std::string get_library_name(std::string & msg_type)
{
  static const std::string DEFAULT_LIBRARY_NAME = "irobot_interfaces_plugin";
  static const std::string namespace_delimiter = "::";

  std::string library_name;
  size_t pos = msg_type.find(namespace_delimiter);
  if (pos == std::string::npos) {
    library_name = DEFAULT_LIBRARY_NAME + "_implementation";
  } else {
    library_name = msg_type.substr(0, pos) + "_implementation";
    msg_type = msg_type.substr(pos + namespace_delimiter.length());
  }

  return library_name;
}

std::shared_ptr<rcpputils::SharedLibrary> get_library(std::string & msg_type)
{
  std::string library_name = get_library_name(msg_type);

  const std::string library_path = rcpputils::get_platform_library_name(library_name);
  auto library = std::make_shared<rcpputils::SharedLibrary>(library_path);

  return library;
}

}  // namespace performance_test_factory
