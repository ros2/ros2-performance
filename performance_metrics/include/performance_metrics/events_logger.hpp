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

#ifndef PERFORMANCE_METRICS__EVENTS_LOGGER_HPP_
#define PERFORMANCE_METRICS__EVENTS_LOGGER_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
#include <chrono>
#include <iomanip>

namespace performance_metrics
{

/**
 * This class implements a synchronous writer for events.
 * It's thread safe and logs each event on a different line of a TSV file.
 */
class EventsLogger
{
public:
  enum EventCode
  {
    discovery,
    late_message,
    too_late_message,
    lost_messages,
    service_unavailable,
    action_unavailable
  };

  struct Event
  {
    std::string caller_name;
    EventCode code;
    std::string description;
  };

  EventsLogger() = delete;

  explicit EventsLogger(const std::string & filename, const bool csv_out = false);

  void set_start_time(std::chrono::high_resolution_clock::time_point t);

  void write_event(const Event & event);

private:
  template<typename T>
  void stream_out(
    std::ostream & stream, const T val, const int space = 15, bool sep_suffix = true);

  std::chrono::high_resolution_clock::time_point m_start_time;
  std::fstream m_file;
  std::string m_filename;
  std::mutex m_writer_mutex;

  // For print formatting
  static const char _p_separator = ' ';
  static const int _p_time_width = 12;
  static const int _p_caller_width = 25;
  static const int _p_code_width = 6;
  static const int _p_desc_width = 20;
  bool _p_csv_out = false;
};

}  // namespace performance_metrics

#endif  // PERFORMANCE_METRICS__EVENTS_LOGGER_HPP_
