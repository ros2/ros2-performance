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

#include <cmath>
#include <cstdint>
#include <iomanip>
#include <map>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "performance_metrics/stat_logger.hpp"

namespace performance_metrics
{

/**
 * @brief Converts a deque of uint64_t values to a formatted string.
 *
 * This function takes a `std::deque` of `uint64_t` integers and returns
 * a string representation in the format: [val1; val2; val3].
 *
 * @param dq The deque of uint64_t values to convert.
 * @return A string representation of the deque.
 */
static std::string dequeToString(const std::deque<uint64_t> & dq)
{
  std::ostringstream oss;
  oss << "[";
  for (size_t i = 0; i < dq.size(); ++i) {
    oss << dq[i] / 1000;
    if (i < dq.size() - 1) {
      oss << "; ";
    }
  }
  oss << "]";
  return oss.str();
}

template<typename T>
void stream_out(
  const bool csv_out,
  std::ostream & stream,
  const T val, const int space,
  bool sep_suffix)
{
  // whether comma or space delimited
  if (csv_out) {
    stream << val << ((sep_suffix) ? "," : "");
  } else {
    const char separator = ' ';
    stream << std::left << std::setw(space) << std::setfill(separator) << val;
  }
}

void log_total_stats(
  uint64_t total_received,
  uint64_t total_lost,
  uint64_t total_late,
  uint64_t total_too_late,
  double average_latency,
  std::ostream & stream,
  const bool csv_out)
{
  const int wide_space = 15;
  const int narrow_space = 10;

  double total_lost_percentage =
    static_cast<double>(total_lost) / (total_received + total_lost) * 100;
  double total_late_percentage =
    static_cast<double>(total_late) / total_received * 100;
  double total_too_late_percentage =
    static_cast<double>(total_too_late) / total_received * 100;

  // log header
  stream_out(csv_out, stream, "received_msgs", wide_space);
  stream_out(csv_out, stream, "mean_us", narrow_space);
  stream_out(csv_out, stream, "late_msgs", narrow_space);
  stream_out(csv_out, stream, "late_perc", narrow_space);
  stream_out(csv_out, stream, "too_late_msgs", wide_space);
  stream_out(csv_out, stream, "too_late_perc", wide_space);
  stream_out(csv_out, stream, "lost_msgs", narrow_space);
  stream_out(csv_out, stream, "lost_perc", narrow_space, false);
  stream << std::endl;

  // log total values
  stream_out(csv_out, stream, total_received, wide_space);
  stream_out(csv_out, stream, average_latency, narrow_space);
  stream_out(csv_out, stream, total_late, narrow_space);
  stream_out(csv_out, stream, total_late_percentage, narrow_space);
  stream_out(csv_out, stream, total_too_late, wide_space);
  stream_out(csv_out, stream, total_too_late_percentage, wide_space);
  stream_out(csv_out, stream, total_lost, narrow_space);
  stream_out(csv_out, stream, total_lost_percentage, narrow_space, false);
  stream << std::endl;
}

void log_trackers_latency_all_stats(
  std::ostream & stream,
  const std::vector<Tracker> & trackers,
  const bool csv_out,
  const std::string & title)
{
  constexpr char separator = ' ';
  constexpr int wide_space = 15;
  constexpr int narrow_space = 10;
  constexpr int extended_space = 25;

  auto log_header = [&stream, wide_space, narrow_space, separator, csv_out](
    const std::string & header_title)
    {
      stream << std::endl;
      stream << header_title << std::endl;
      stream_out(csv_out, stream, "node", wide_space);
      stream_out(csv_out, stream, "topic", wide_space);
      stream_out(csv_out, stream, "size_b", narrow_space);
      stream_out(csv_out, stream, "received_msgs", wide_space);
      stream_out(csv_out, stream, "late_msgs", narrow_space);
      stream_out(csv_out, stream, "too_late_msgs", wide_space);
      stream_out(csv_out, stream, "lost_msgs", narrow_space);
      stream_out(csv_out, stream, "mean_us", narrow_space);
      stream_out(csv_out, stream, "sd_us", narrow_space);
      stream_out(csv_out, stream, "min_us", narrow_space);
      stream_out(csv_out, stream, "max_us", narrow_space);
      stream_out(csv_out, stream, "freq_hz", narrow_space);
      stream_out(csv_out, stream, "throughput_Kb_per_sec", extended_space);
      stream_out(csv_out, stream, "all_lat", narrow_space, false);

      stream << std::endl;
    };

  auto log_stats_line = [&stream, wide_space, narrow_space, separator, csv_out](
    const Tracker & tracker)
    {
      stream_out(csv_out, stream, tracker.get_node_name(), wide_space);
      stream_out(csv_out, stream, tracker.get_entity_name(), wide_space);
      stream_out(csv_out, stream, tracker.size(), narrow_space);
      stream_out(csv_out, stream, tracker.received(), wide_space);
      stream_out(csv_out, stream, tracker.late(), narrow_space);
      stream_out(csv_out, stream, tracker.too_late(), wide_space);
      stream_out(csv_out, stream, tracker.lost(), narrow_space);
      stream_out(csv_out, stream, tracker.stat().mean() / 1000.0, narrow_space);
      stream_out(csv_out, stream, tracker.stat().stddev() / 1000.0, narrow_space);
      stream_out(csv_out, stream, tracker.stat().min() / 1000.0, narrow_space);
      stream_out(csv_out, stream, tracker.stat().max() / 1000.0, narrow_space);
      stream_out(csv_out, stream, tracker.frequency(), narrow_space);
      stream_out(csv_out, stream, (tracker.throughput() / 1024), extended_space);
      stream_out(csv_out, stream, dequeToString(tracker.get_all_latency()), narrow_space, false);

      stream << std::endl;
    };

  if (trackers.empty()) {
    return;
  }

  log_header(title);
  for (const auto & tracker : trackers) {
    log_stats_line(tracker);
  }
}

void log_trackers_sent_vs_received(
  std::ostream & stream,
  const std::vector<Tracker> & pub_trackers,
  const std::vector<Tracker> & sub_trackers,
  const bool csv_out,
  double duration_sec)
{
  constexpr int wide_space = 15;
  constexpr int narrow_space = 12;

  struct Agg
  {
    uint64_t sent = 0;
    uint64_t received = 0;
    uint64_t gap_lost = 0;
    double cfg_hz = 0.0;
    int n_pub = 0;
    int n_sub = 0;
  };

  // Group by topic name. Correct for 1:1 pub/sub topologies; for fan-out
  // (1 pub -> N subs) received messages are summed across subs,
  // so delivered_perc can exceed 100 and true_lost go negative.
  std::map<std::string, Agg> by_topic;

  for (const auto & t : pub_trackers) {
    auto & a = by_topic[t.get_entity_name()];
    a.sent += t.sent();
    a.cfg_hz = t.frequency();
    a.n_pub += 1;
  }
  for (const auto & t : sub_trackers) {
    auto & a = by_topic[t.get_entity_name()];
    a.received += t.received();
    a.gap_lost += t.lost();
    a.n_sub += 1;
  }

  if (by_topic.empty()) {
    return;
  }

  stream << std::endl;
  stream << "Sent vs received messages:" << std::endl;
  // expected = cfg_hz * duration (nominal target). Decomposes the loss:
  //   unsent    = expected - sent     (publisher could not fire fast enough)
  //   true_lost = sent - received     (dropped in transport / by the executor)
  // gap_lost is the old sequence-gap counter, kept for comparison.
  stream_out(csv_out, stream, "topic", wide_space);
  stream_out(csv_out, stream, "n_pub", narrow_space);
  stream_out(csv_out, stream, "n_sub", narrow_space);
  stream_out(csv_out, stream, "cfg_hz", narrow_space);
  stream_out(csv_out, stream, "expected", narrow_space);
  stream_out(csv_out, stream, "sent", narrow_space);
  stream_out(csv_out, stream, "unsent", narrow_space);
  stream_out(csv_out, stream, "received", narrow_space);
  stream_out(csv_out, stream, "delivered_perc", wide_space);
  stream_out(csv_out, stream, "true_lost", narrow_space);
  stream_out(csv_out, stream, "gap_lost", narrow_space, false);
  stream << std::endl;

  uint64_t tot_expected = 0;
  uint64_t tot_sent = 0;
  uint64_t tot_recv = 0;
  uint64_t tot_gap = 0;
  int64_t tot_unsent = 0;
  int64_t tot_true_lost = 0;

  for (const auto & kv : by_topic) {
    const auto & a = kv.second;
    const uint64_t expected =
      (duration_sec > 0.0) ? static_cast<uint64_t>(std::llround(a.cfg_hz * duration_sec)) : 0;
    const int64_t unsent =
      static_cast<int64_t>(expected) - static_cast<int64_t>(a.sent);
    const int64_t true_lost =
      static_cast<int64_t>(a.sent) - static_cast<int64_t>(a.received);
    const double delivered = (a.sent > 0) ? (100.0 * a.received / a.sent) : 0.0;

    stream_out(csv_out, stream, kv.first, wide_space);
    stream_out(csv_out, stream, a.n_pub, narrow_space);
    stream_out(csv_out, stream, a.n_sub, narrow_space);
    stream_out(csv_out, stream, a.cfg_hz, narrow_space);
    stream_out(csv_out, stream, expected, narrow_space);
    stream_out(csv_out, stream, a.sent, narrow_space);
    stream_out(csv_out, stream, unsent, narrow_space);
    stream_out(csv_out, stream, a.received, narrow_space);
    stream_out(csv_out, stream, delivered, wide_space);
    stream_out(csv_out, stream, true_lost, narrow_space);
    stream_out(csv_out, stream, a.gap_lost, narrow_space, false);
    stream << std::endl;

    tot_expected += expected;
    tot_sent += a.sent;
    tot_recv += a.received;
    tot_gap += a.gap_lost;
    tot_unsent += unsent;
    tot_true_lost += true_lost;
  }

  const double tot_delivered =
    (tot_sent > 0) ? (100.0 * tot_recv / tot_sent) : 0.0;

  stream_out(csv_out, stream, "TOTAL", wide_space);
  stream_out(csv_out, stream, "", narrow_space);
  stream_out(csv_out, stream, "", narrow_space);
  stream_out(csv_out, stream, "", narrow_space);
  stream_out(csv_out, stream, tot_expected, narrow_space);
  stream_out(csv_out, stream, tot_sent, narrow_space);
  stream_out(csv_out, stream, tot_unsent, narrow_space);
  stream_out(csv_out, stream, tot_recv, narrow_space);
  stream_out(csv_out, stream, tot_delivered, wide_space);
  stream_out(csv_out, stream, tot_true_lost, narrow_space);
  stream_out(csv_out, stream, tot_gap, narrow_space, false);
  stream << std::endl;
}

void log_trackers_latency_total_stats(
  std::ostream & stream,
  const std::vector<Tracker> & trackers,
  const bool csv_out)
{
  uint64_t total_received = 0;
  uint64_t total_lost = 0;
  uint64_t total_late = 0;
  uint64_t total_too_late = 0;
  double total_latency = 0;

  // collect total data
  for (const auto & tracker : trackers) {
    total_received += tracker.received();
    total_lost += tracker.lost();
    total_late += tracker.late();
    total_too_late += tracker.too_late();
    total_latency += tracker.received() * tracker.stat().mean();
  }

  double average_latency = total_latency / total_received / 1000.0;

  log_total_stats(
    total_received, total_lost, total_late, total_too_late,
    average_latency, stream, csv_out);
}

uint64_t get_trackers_avg_latency(const std::vector<const Tracker *> & trackers)
{
  uint64_t total_received = 0;
  double total_latency = 0;

  for (const auto & tracker : trackers) {
    total_received += tracker->delta_received();
    total_latency += tracker->delta_received() * tracker->delta_stat().mean();
    // Reset values after read
    tracker->reset_delta_received();
    tracker->reset_delta_stat();
  }

  if (total_received) {
    return std::round(total_latency / total_received / 1000.0);
  } else {
    return 0;
  }
}

}  // namespace performance_metrics
