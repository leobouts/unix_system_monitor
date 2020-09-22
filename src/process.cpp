#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"


using std::string;
using std::to_string;
using std::vector;

//when initialize everything, memory crash
Process::Process(int p) {
  pid = p;
  user = LinuxParser::User(p);
  command = LinuxParser::Command(p);
  uptime = LinuxParser::UpTime(p);
  ram = LinuxParser::Ram(p);
  cpuUsage = LinuxParser::CpuProc(p);
  
}

int Process::Pid() {return pid;}
float Process::CpuUtilization() { return cpuUsage;}
string Process::Command() {return command;}
string Process::Ram() {return ram;}
string Process::User() {return user;}
long int Process::UpTime() {return uptime;}



