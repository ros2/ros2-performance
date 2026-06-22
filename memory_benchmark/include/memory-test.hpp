#pragma once

#include <string>
#include <iostream>
#include <iomanip>

#include <sys/resource.h>
#include <sys/types.h>

inline void print_header()
{
  // Print header in CSV format
  std::cout << "Entity,N,RSS,DELTA" << std::endl;
}

inline void print_rss(const std::string & description, size_t number)
{
  struct rusage usage;
  static long last_rss = 0;

  getrusage(RUSAGE_SELF, &usage);

  long current_rss = usage.ru_maxrss;
  long delta_rss = current_rss - last_rss;

  // Print values in CSV format
  std::cout << description << ","
            << number << ","
            << current_rss << ","
            << delta_rss << std::endl;

  last_rss = usage.ru_maxrss;
}
