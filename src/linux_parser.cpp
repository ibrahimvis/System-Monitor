#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>  // for debugging
#include <string>
#include <vector>


using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float shmem = 0.0;
  float sreclaimable = 0.0;  //, buffersMem = 0.0;
  float usedMem = 0.0, cachedMem = 0.0, totalMem = 0.0, freeMem = 0.0;

  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');

      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        char chars[] = {'_', ' ', 'k', 'B'};
        for (char c : chars) {
          value.erase(remove(value.begin(), value.end(), c), value.end());
        }

        switch (key[0]) {
          case 'M':
            if (key == "MemTotal") {
              totalMem = stof(value);
            } else if (key == "MemFree") {
              freeMem = stof(value);
            }
            break;
          // case 'B':
          //   if (key == "Buffers") {
          //     buffersMem = stof(value);
          //   }
          //   break;
          case 'C':
            if (key == "Cached") {
              cachedMem = stof(value);
            }
            break;
          case 'S':
            if (key == "Shmem") {
              shmem = stof(value);
            }
            if (key == "SReclaimable") {
              sreclaimable = stof(value);
            }
            break;
        }
      }
    }
  }

  cachedMem = cachedMem + sreclaimable - shmem;
  usedMem = totalMem - freeMem - cachedMem;
  return (usedMem / totalMem);
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long upTime = 0;
  long suspendL;  //, idleL;
  string line, suspend, idle;

  std::ifstream file(kProcDirectory + kUptimeFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream line_s(line);

    line_s >> suspend;  // >> idle;
    suspendL = std::stol(suspend);
    // idleL = std::stol(idle);

    upTime = suspendL;
  }

  return upTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  long total = 0;
  for (string jiff : jiffies) {
    total += std::stoi(jiff);
  }

  return total;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  long cpu_usage;

  std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);

    string token;
    int utime, stime, cutime, cstime, starttime;

    /*

    #14 utime - CPU time spent in user code, measured in clock ticks
    #15 stime - CPU time spent in kernel code, measured in clock ticks
    #16 cutime - Waited-for children's CPU time spent in user code (in clock
    ticks) #17 cstime - Waited-for children's CPU time spent in kernel code (in
    clock ticks) #22 starttime - Time when the process started, measured in
    clock ticks

    */
    for (int i = 0; i < 22 && linestream >> token; i++) {
      switch (i) {
        case 13:
          utime = std::stoi(token);
          // std::cout << i << " " << token << std::endl;
          break;
        case 14:
          stime = std::stoi(token);
          // std::cout << i << " " << token << std::endl;
          break;
        case 15:
          cutime = std::stoi(token);
          // std::cout << i << " " << token << std::endl;
          break;
        case 16:
          cstime = std::stoi(token);
          // std::cout << i << " " << token << std::endl;
          break;
        case 21:
          starttime = std::stoi(token);
          // std::cout << i << " " << token << std::endl;
          break;
      }
      // std::cout << i << " " << token << std::endl;
    }

    double total_time = utime + stime + cutime + cstime;
    // std::cout << "Total: " << total_time << std::endl;
    double seconds = LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
    // std::cout << "Seconds: " << seconds << std::endl;
    cpu_usage = (total_time / sysconf(_SC_CLK_TCK)) / seconds;
    // std::cout << "cpu Usage: " << cpu_usage << std::endl;
  }
  if (cpu_usage < 0)
    return 0;
  else
    return cpu_usage;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  long total = 0;
  for (int i = 0; i < 8; i++) {
    if (i != kIdle_ && i != kIOwait_) total += std::stol(jiffies[i]);
  }

  return (total);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  long idle = std::stol(jiffies[kIdle_]) + std::stol(jiffies[kIOwait_]);

  return idle;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpu{};
  string line;

  std::ifstream file(kProcDirectory + kStatFilename);

  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream line_s(line);
    while (!line_s.eof()) {
      string token;
      line_s >> token;
      cpu.push_back(token);
    }

    cpu.erase(cpu.begin());
  }

  return cpu;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line, key, value;
  float totalProcesses = 0.0;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      // line.replace(line.find(" "), line.length(), "_");

      std::istringstream line_s(line);

      while (line_s >> key >> value) {
        if (key == "processes") {
          totalProcesses = stof(value);
        }
      }
    }
  }

  return totalProcesses;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line, key, value;
  float runningProcesses = 0.0;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      // line.replace(line.find(" "), line.length(), "_");

      std::istringstream line_s(line);

      while (line_s >> key >> value) {
        if (key == "procs_running") {
          runningProcesses = stof(value);
        }
      }
    }
  }

  return runningProcesses;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  string key, value;
  std::ifstream file(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

  std::getline(file, line);

  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  string line;
  string key, value;
  std::ifstream file(kProcDirectory + std::to_string(pid) + kStatusFilename);

  while (std::getline(file, line)) {
    std::istringstream linestream(line);

    linestream >> key >> value;
    if (key == "VmSize:") {
      value = to_string(std::stol(value) / 1000);
      break;
    }
  }

  return value;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string line;
  string uid = "none";

  std::ifstream file(kProcDirectory + to_string(pid) + kStatusFilename);

  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream line_s(line);
      string name, value, extra;

      line_s >> name >> value >> extra;
      if (name == "Uid:") {
        uid = value;
        break;
      }
    }
  }

  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = Uid(pid);

  string line;
  string user;

  std::ifstream file(kPasswordPath);
  if (file.is_open()) {
    while (getline(file, line)) {
      // std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');

      std::istringstream linestream(line);
      string passwd, uid2, gid, comment, home, bash;
      linestream >> user >> passwd >> uid2 >> gid >> comment >> home >> bash;

      if (uid2 == uid) {
        break;
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  long int uptime = 0;

  std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);

    string token;

    for (int i = 0; i < 22 && linestream >> token; i++) {
      if (i == 21) {
        uptime = std::stol(token) / sysconf(_SC_CLK_TCK);
        break;
      }
    }

    return uptime;
  }

  return uptime;
}