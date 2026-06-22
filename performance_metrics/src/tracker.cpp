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

#include <algorithm>
#include <memory>
#include <sstream>

#include "performance_metrics/tracker.hpp"

namespace performance_metrics
{

void Tracker::scan(
  const performance_test_msgs::msg::PerformanceHeader & header,
  const rclcpp::Time & now,
  std::shared_ptr<EventsLogger> elog)
{
  // Compute latency
  rclcpp::Time stamp(header.stamp.sec, header.stamp.nanosec, RCL_ROS_TIME);
  auto lat = std::chrono::nanoseconds((now - stamp).nanoseconds());
  uint64_t lat_us = lat.count() / 1000;

  if (lat.count() < 0) {
    std::cout << "Negative latency detected: " << lat.count() << " nanoseconds" << std::endl;
  }

  // store the last latency to be read from node
  m_last_latency = lat_us;

  bool late = false;
  bool too_late = false;

  if (m_tracking_options.is_enabled) {
    // Check if we received the correct message. The assumption here is
    // that the messages arrive in chronological order
    if (header.tracking_number == m_tracking_number_count) {
      m_tracking_number_count++;
    } else {
      // We missed some mesages...
      int64_t n_lost = header.tracking_number - m_tracking_number_count;
      m_lost_messages += n_lost;
      m_tracking_number_count = header.tracking_number + 1;

      // Log the event
      if (elog != nullptr) {
        EventsLogger::Event ev;
        std::stringstream description;
        ev.caller_name = m_topic_srv_name + "->" + m_node_name;
        ev.code = EventsLogger::EventCode::lost_messages;

        if (n_lost == 1) {
          description << "msg " << header.tracking_number - 1 << " lost.";
        } else {
          int64_t span_lost = header.tracking_number - 1 + n_lost;
          description << "msgs " << header.tracking_number - 1 << " to " << span_lost << " lost.";
        }
        ev.description = description.str();
        elog->write_event(ev);
      }
    }

    // Check if the message latency qualifies the message as a lost or late message.
    const int period_us = 1000000 / header.frequency;
    const unsigned int latency_late_threshold_us = std::min(
      m_tracking_options.late_absolute_us,
      m_tracking_options.late_percentage * period_us / 100);
    const unsigned int latency_too_late_threshold_us = std::min(
      m_tracking_options.too_late_absolute_us,
      m_tracking_options.too_late_percentage * period_us / 100);

    too_late = lat_us > latency_too_late_threshold_us;
    late = lat_us > latency_late_threshold_us && !too_late;

    if (late) {
      if (elog != nullptr) {
        // Create a description for the event
        std::stringstream description;
        description << "msg " << header.tracking_number << " late. " <<
          lat_us << "us > " << latency_late_threshold_us << "us";

        EventsLogger::Event ev;
        ev.caller_name = m_topic_srv_name + "->" + m_node_name;
        ev.code = EventsLogger::EventCode::late_message;
        ev.description = description.str();

        elog->write_event(ev);
      }
      m_late_messages++;
    }

    if (too_late) {
      if (elog != nullptr) {
        // Create a descrption for the event
        std::stringstream description;
        description << "msg " << header.tracking_number << " too late. " <<
          lat_us << "us > " << latency_too_late_threshold_us << "us";

        EventsLogger::Event ev;
        ev.caller_name = m_topic_srv_name + "->" + m_node_name;
        ev.code = EventsLogger::EventCode::too_late_message;
        ev.description = description.str();

        elog->write_event(ev);
      }
      m_too_late_messages++;
    }
  }

  // Compute statistics with new sample
  this->add_sample(now, lat_us, header.size, header.frequency);

  m_received_messages++;
  m_delta_received_messages++;
}

void Tracker::add_sample(
  const rclcpp::Time & now,
  uint64_t latency_sample,
  size_t size,
  float frequency)
{
  // If this is first message received store some info about it
  if (m_stat.n() == 0) {
    m_data_size = size;
    m_frequency = frequency;
    m_first_msg_time = now;
  }

  m_last_msg_time = now;
  m_stat.add_sample(latency_sample);
  m_delta_stat.add_sample(latency_sample);
  // Add latency value to the deque
  m_all_latency.push_back(latency_sample);
}

uint32_t Tracker::get_and_update_tracking_number()
{
  uint32_t old_number = m_tracking_number_count;
  m_tracking_number_count++;
  return old_number;
}

double Tracker::throughput() const
{
  // We compute max because currently publishers update only n() and not m_received_messages,
  // but for subscriptions n() will not include messages received too late.
  uint64_t num_msg_received = std::max(m_stat.n(), m_received_messages);
  if (num_msg_received < 2) {
    return 0.0;
  }

  rclcpp::Duration msgs_received_interval = m_last_msg_time - m_first_msg_time;
  double sample_rate_sec = (num_msg_received - 1) / msgs_received_interval.seconds();
  double throughput = sample_rate_sec * m_data_size;

  return throughput;
}

std::ostream & operator<<(std::ostream & os, const Tracker::Options & opts)
{
  os << "late_percentage: " << opts.late_percentage << std::endl;
  os << "late_absolute_us: " << opts.late_absolute_us << std::endl;
  os << "too_late_percentage: " << opts.too_late_percentage << std::endl;
  os << "too_late_absolute_us: " << opts.too_late_absolute_us << std::endl;

  return os;
}

}  // namespace performance_metrics
