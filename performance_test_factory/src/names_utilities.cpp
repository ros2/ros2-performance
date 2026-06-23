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

#include <iterator>
#include <regex>
#include <sstream>
#include <string>

#include "performance_test_factory/names_utilities.hpp"

std::string performance_test_factory::id_to_node_name(int id)
{
  std::stringstream ss;
  ss << "node_";
  ss << id;
  return ss.str();
}

std::string performance_test_factory::id_to_service_name(int id)
{
  std::stringstream ss;
  ss << "service_";
  ss << id;
  return ss.str();
}

std::string performance_test_factory::id_to_topic_name(int id)
{
  std::stringstream ss;
  ss << "topic_";
  ss << id;
  return ss.str();
}

int performance_test_factory::item_name_to_id(std::string name)
{
  // if the name is in the format "node_7", "topic_1", "service_12" extract the number from it
  // otherwise create an hash code from the string

  // use only one regex with positive lookahead and start string delimiter
  // directly call replace and check if length is changed

  std::regex e("(node|topic|service)_[[:digit:]]+");

  if (std::regex_match(name, e)) {
    std::regex e2("(node|topic|service)_");
    std::stringstream number_string;
    std::regex_replace(
      std::ostream_iterator<char>(number_string), name.begin(), name.end(), e2,
      "");
    // This will FAIL if the provided number starts with 0 e.g. "topic_01" but it should not happen
    return std::stoi(number_string.str());
  }

  // fallback if regex condition is not satisfied
  return std::hash<std::string>()(name);
}
