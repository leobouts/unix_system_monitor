#include "processor.h"
#include "linux_parser.h"
#include <iostream>

Processor::Processor() : prevNonIdle(0), prevIdle(0){};

// Return the aggregate CPU utilization
float Processor::Utilization() {
  
  // total cpu time since boot = user+nice+system+idle+iowait+irq+softirq+steal
  float nonIdleCpu = LinuxParser::ActiveJiffies();
  // idle time since boot = idle + iowait
  float idleCpu = LinuxParser::IdleJiffies();
    
  
  // calculate the cpu usage since last update
  float prevTotal = prevNonIdle + prevIdle;
  float Total = nonIdleCpu + idleCpu;
  
  float totald = Total - prevTotal;
  float idled = idleCpu - prevIdle;
  
  float diffUsage = (totald - idled) / totald;
  // remember the total and idle times for next check
  prevNonIdle = nonIdleCpu;
  prevIdle = idleCpu;
  
  return diffUsage;

}