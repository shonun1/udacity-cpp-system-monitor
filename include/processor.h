#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  std::vector<long> _prev_cpu_util{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
};

#endif