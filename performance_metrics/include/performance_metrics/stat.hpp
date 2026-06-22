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

#ifndef PERFORMANCE_METRICS__STAT_HPP_
#define PERFORMANCE_METRICS__STAT_HPP_

#include <cmath>
#include <utility>

namespace performance_metrics
{

// Use shifted data variance algorithm
// https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance
template<typename T>
class Stat
{
public:
  void add_sample(T x)
  {
    if (_n == 0) {
      _max = x;
      _min = x;
      K = x;
    }

    if (x > _max) {
      _max = x;
    }

    if (x < _min) {
      _min = x;
    }

    _n++;
    Ex += x - K;
    Ex2 += (x - K) * (x - K);
  }

  double mean() const
  {
    if (_n == 0) {
      return std::nan("");
    }
    return K + Ex / _n;
  }

  double stddev() const
  {
    if (_n == 0) {
      return std::nan("");
    }
    if (_n == 1) {
      return 0;
    }
    return std::sqrt((Ex2 - (Ex * Ex) / _n) / (_n));
  }

  double max() const
  {
    if (_n == 0) {
      return std::nan("");
    }
    return _max;
  }

  double min() const
  {
    if (_n == 0) {
      return std::nan("");
    }
    return _min;
  }

  uint64_t n() const {return _n;}

private:
  double _max;
  double _min;
  double K;
  double Ex = 0;
  double Ex2 = 0;
  uint64_t _n = 0;
};

}  // namespace performance_metrics

#endif  // PERFORMANCE_METRICS__STAT_HPP_
