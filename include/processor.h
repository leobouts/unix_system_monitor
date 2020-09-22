#ifndef PROCESSOR_H
#define PROCESSOR_H
#include <vector>
#include <string>

class Processor {
 public:
  float Utilization(); 
  Processor();
 private:
  
  // previous total cpu time since boot =
  // user+nice+system+idle+iowait+irq+softirq+steal
  long prevNonIdle;
  // previous idle time since boot = idle + iowait
  long prevIdle;

  // convert the given string vector into a long vector
  std::vector<long> convertToLong(std::vector<std::string> values);
};

#endif