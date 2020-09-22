#ifndef PROCESS_H
#define PROCESS_H

#include <string>
#include "linux_parser.h"
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int);
  int Pid();                               // TODO: See src/process.cpp
  std::string User();                      // TODO: See src/process.cpp
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

 private:
  int pid;
  std::string user;
  std::string command;
  float cpuUsage;
  std::string ram;
  long uptime;
  
  float instantiateCpu(int);
  std::string instantiateUser(int);
  std::string instantiateCommand(int);
  std::string instantiateRam(int);
  long int instantiateUpTime(int);

};

#endif