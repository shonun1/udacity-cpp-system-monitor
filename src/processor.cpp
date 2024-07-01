#include "processor.h"

#include <string>

#include "linux_parser.h"

using LinuxParser::CPUStates;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  std::vector<long> cpu_values;
  for (auto el : LinuxParser::CpuUtilization()) {
    cpu_values.push_back(std::stol(el));
  }

  // algorithm based on
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  long prev_idle =
      _prev_cpu_util[CPUStates::kIdle_] + _prev_cpu_util[CPUStates::kIOwait_];
  long idle = LinuxParser::IdleJiffies();

  long prev_non_idle =
      _prev_cpu_util[CPUStates::kUser_] + _prev_cpu_util[CPUStates::kNice_] +
      _prev_cpu_util[CPUStates::kSystem_] + _prev_cpu_util[CPUStates::kIRQ_] +
      _prev_cpu_util[CPUStates::kSoftIRQ_] + _prev_cpu_util[CPUStates::kSteal_];
  long non_idle = LinuxParser::ActiveJiffies();

  long prev_total = prev_idle + prev_non_idle;
  long total = idle + non_idle;
  long idled = idle - prev_idle;
  long totald = total - prev_total;
  long result_num = totald - idled;
  float result = float(result_num) / float(totald);

  _prev_cpu_util = cpu_values;

  return result;
}