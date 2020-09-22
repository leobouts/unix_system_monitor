#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

//read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

//read data from the filesystem
string LinuxParser::Kernel() {
  string os, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    //dump first two words
    linestream >> os;
    linestream >> os;
    linestream >> version;
  }
  return version;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


float LinuxParser::MemoryUtilization() {
  string line;
  string name,value,measure;
  
  vector<float> mem_info; 
  
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    // read the elements in the file into a vector 
    // only need the first two   
    linestream >> name >> value >> measure;
    mem_info.push_back(std::stof(value));

    std::getline(stream, line);
    std::istringstream stream(line);

    stream >> name >> value >> measure;
    mem_info.push_back(std::stof(value));
    
  }
  
  float memory_total = mem_info[0];
  float memory_free = mem_info[1]; 
  float utilization = (memory_total - memory_free) / memory_total;
  
  return utilization;
  
}

//Read and return the system uptime
long LinuxParser::UpTime() {
  float uptime, suspend;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> suspend;
  }
  return uptime;
}

//Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  string line, cpu;
  long value, jiffies;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    //remove the name
    linestream >> cpu;
    while(linestream >> value){
      jiffies = jiffies + value;
    }
  }
  return jiffies;
}

// Read and return the number of active jiffies for a PID
float  LinuxParser::CpuProc(int pid) {
  //index: utime, stime, cutime, cstime
  string line;
  string value;
  string path = LinuxParser::kProcDirectory + "/" + std::to_string(pid) + LinuxParser::kStatFilename;  
  std::ifstream filestream(path);
  vector<float> cpuValues{};
  if (filestream.is_open()) {
    string line;
    float time = 0;
    string value;
    std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) + kStatFilename);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        std::istringstream linestream(line);
        for (int i = 1; i < 23; i++) {
          linestream >> value;
          if (i == 14 || i == 15 || i == 16 || i == 17 || i == 22) {

            time = std::stof(value) / sysconf(_SC_CLK_TCK);
            cpuValues.push_back(time);
          }
        }
      } 
    }
  }
  if(cpuValues.size() == 5) {
    float totaltime = cpuValues[0] + cpuValues[1] + cpuValues[2] + cpuValues[3];
    float seconds = LinuxParser::UpTime() - cpuValues[4];
  
    return totaltime / seconds;
  }else{
    return 0;
  }
}

//Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string line, name;
  long value;
  vector<int> cpu;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    //remove the name
    linestream >> name;
    while(linestream >> value){
      cpu.push_back(value);
    }
  }
  // user + nice + system + irq + softirq + steal
  long active = cpu[0] + cpu[1] + cpu[2] + cpu[5] + cpu[6] + cpu[7];
  return active;
}

//Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line, name;
  long value;
  vector<int> cpu;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    //remove the name
    linestream >> name;
    while(linestream >> value){
      cpu.push_back(value);
    }
  }
  //idle + iowait
  return cpu[3] + cpu[4];
}

std::vector<std::string> LinuxParser::CpuUtilization() {
  vector<string> cpuValues{};
  string line;
  string key;
  string vuser, vnice, vsystem, vidle, viowait, virq, vsoftirq, vsteal, vguest,
      vguest_nice;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> vuser >> vnice >> vsystem >> vidle >>
             viowait >> virq >> vsoftirq >> vsteal >> vguest >> vguest_nice) {
        if (key == "cpu") {
          cpuValues.push_back(vuser);
          cpuValues.push_back(vnice);
          cpuValues.push_back(vsystem);
          cpuValues.push_back(vidle);
          cpuValues.push_back(viowait);
          cpuValues.push_back(virq);
          cpuValues.push_back(vsoftirq);
          cpuValues.push_back(vsteal);
          cpuValues.push_back(vguest);
          cpuValues.push_back(vguest_nice);
          return cpuValues;
        }
      }
    }
  }
  return {};
}

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          return std::stol(value);
        }
      }
    }
  }
  return 0;
}
  
//Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stol(value);
        }
      }
    }
  }
  return 0;
}


//Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string command;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if(stream.is_open()){
    std::getline(stream,command);
  }
  return command;
}


//Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line,key;
  string value = "";
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          long conv = std::stol(value) / 1000;
          value = std::to_string(conv);
          return value;
        }
      }
    }
  }
  return value;
}



//Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string name = "Uid";
  string title, value;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  string line;
  while (std::getline(stream, line)) {
    if (line.compare(0, name.size(), name) == 0) {
      std::istringstream linestream(line);
      linestream >> title >> value;
      return value;
    }
  }
  return "";
}

//Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string name = "x:" + Uid(pid);
  string title, value;
  std::ifstream stream(kPasswordPath);
  string line;
  while(std::getline(stream, line)){
    if(line.find(name) != std::string::npos){
      return line.substr(0, line.find(":"));
    }
  }
  return "";
}

//Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  long uptime = 0;
  string value;
  std::ifstream filestream(kProcDirectory + "/" + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      for (int i = 1; i <= 22; i++) {
        linestream >> value;
        if (i == 22) {
          uptime = std::stol(value) / sysconf(_SC_CLK_TCK);
          return uptime;
        }
      }
    }
  }
  return uptime;
}