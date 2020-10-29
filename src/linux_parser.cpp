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


vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {

    if (file->d_type == DT_DIR) {

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
  float shmem = 0.0;
  float sreclaimable = 0.0;
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
            if (key == filterMemTotalString) {
              totalMem = stof(value);
            } else if (key == filterMemFreeString) {
              freeMem = stof(value);
            }
            break;
          case 'C':
            if (key == filterCachedString) {
              cachedMem = stof(value);
            }
            break;
          case 'S':
            if (key == filterShmemString) {
              shmem = stof(value);
            }
            if (key == filterSRecString) {
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


long LinuxParser::UpTime() {
  long upTime = 0;
  long suspendL;
  string line, suspend, idle;

  std::ifstream file(kProcDirectory + kUptimeFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream line_s(line);

    line_s >> suspend;
    suspendL = std::stol(suspend);

    upTime = suspendL;
  }

  return upTime;
}


long LinuxParser::Jiffies() {
  vector<string> jiffies = CpuUtilization();
  long total = 0;
  for (string jiff : jiffies) {
    total += std::stoi(jiff);
  }

  return total;
}

long LinuxParser::ActiveJiffies(int pid) {
  string line;
  float cpu_usage;

  std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);
  if (file.is_open()) {
    std::getline(file, line);
    std::istringstream linestream(line);

    string token;
    int utime, stime, cutime, cstime, starttime;

    for (int i = 0; i < 22 && linestream >> token; i++) {
      switch (i) {
        case 13:
          utime = std::stoi(token);
          
          break;
        case 14:
          stime = std::stoi(token);
          
          break;
        case 15:
          cutime = std::stoi(token);
          
          break;
        case 16:
          cstime = std::stoi(token);
          
          break;
        case 21:
          starttime = std::stoi(token);
          
          break;
      }
      
    }

    float total_time = utime + stime + cutime + cstime;

    float seconds = UpTime() - (starttime / sysconf(_SC_CLK_TCK));

    cpu_usage = 100 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);

  }
  
  return cpu_usage;
}


long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = CpuUtilization();
  long total = 0;
  for (int i = 0; i < 8; i++) {
    if (i != kIdle_ && i != kIOwait_) total += std::stol(jiffies[i]);
  }

  return (total);
}


long LinuxParser::IdleJiffies() {
  vector<string> jiffies = CpuUtilization();
  long idle = std::stol(jiffies[kIdle_]) + std::stol(jiffies[kIOwait_]);

  return idle;
}


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


int LinuxParser::TotalProcesses() {
  string line, key, value;
  float totalProcesses = 0.0;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {

      std::istringstream line_s(line);

      while (line_s >> key >> value) {
        if (key == filterProcesses) {
          totalProcesses = stof(value);
        }
      }
    }
  }

  return totalProcesses;
}

int LinuxParser::RunningProcesses() {
  string line, key, value;
  float runningProcesses = 0.0;
  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {

      std::istringstream line_s(line);

      while (line_s >> key >> value) {
        if (key == filterRunningProcesses) {
          runningProcesses = stof(value);
        }
      }
    }
  }

  return runningProcesses;
}

string LinuxParser::Command(int pid) {
  string line;
  string key, value;
  std::ifstream file(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

  std::getline(file, line);

  return line;
}

string LinuxParser::Ram(int pid) {
  string line;
  string key, value;
  std::ifstream file(kProcDirectory + std::to_string(pid) + kStatusFilename);

  while (std::getline(file, line)) {
    std::istringstream linestream(line);

    linestream >> key >> value;
    if (key == filterProcMem) {
      value = to_string(std::stol(value) / 1000);
      break;
    }
  }

  return value;
}

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

string LinuxParser::User(int pid) {
  string uid = Uid(pid);

  string line;
  string user;

  std::ifstream file(kPasswordPath);
  if (file.is_open()) {
    while (getline(file, line)) {

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