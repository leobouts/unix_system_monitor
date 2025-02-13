#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>


#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;



//Return the system's CPU
Processor& System::Cpu() { return cpu; }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  
  processes.clear(); // make sure that the vector is clear 

  // read process IDs from file system and generate Vector
  auto pids = LinuxParser::Pids();
  
  for (int item : pids) {
    Process proc(item);
    processes.push_back(proc);
  }

  return processes;
}


//Return the system's kernel identifier (string)
std::string System::Kernel() { 
  return LinuxParser::Kernel();
}

//Return the system's memory utilization
float System::MemoryUtilization() { 
  return LinuxParser::MemoryUtilization();
}

//Return the operating system name
std::string System::OperatingSystem() { 
  return LinuxParser::OperatingSystem();
}

//Return the number of processes actively running on the system
int System::RunningProcesses() { 
  return LinuxParser::RunningProcesses();
}

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { 
  return LinuxParser::TotalProcesses();
}

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { 
  return LinuxParser::UpTime();
}