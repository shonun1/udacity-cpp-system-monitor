#include "format.h"

#include <string>

#define HOUR 3600
#define MINUTE 60

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hours = seconds / HOUR;
  int minutes = (seconds % HOUR) / MINUTE;
  int remaining_seconds = seconds % MINUTE;
  string minutes_prefix = minutes < 10 ? "0" : "";
  string seconds_prefix = remaining_seconds < 10 ? "0" : "";

  return std::to_string(hours) + ":" + minutes_prefix +
         std::to_string(minutes) + ":" + seconds_prefix +
         std::to_string(remaining_seconds);
}