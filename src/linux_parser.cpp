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

  filestream.close();

  return value;
}

string LinuxParser::Kernel() {
  string os;
  string version;
  string kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }

  stream.close();

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
  try {
    float shmem = 0.0;
    float sreclaimable = 0.0;
    float usedMem = 0.0;
    float cachedMem = 0.0;
    float totalMem = 0.0;
    float freeMem = 0.0;

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

    filestream.close();

    return (usedMem / totalMem);
  } catch (...) {
    return 0;
  }
}

long LinuxParser::UpTime() {
  try {
    long upTime = 0;
    long suspendL = 0;
    string line;
    string suspend;
    string idle;

    std::ifstream file(kProcDirectory + kUptimeFilename);
    if (file.is_open()) {
      std::getline(file, line);
      std::istringstream line_s(line);

      line_s >> suspend;
      suspendL = std::stol(suspend);

      upTime = suspendL;
    }

    file.close();

    return upTime;
  } catch (...) {
    return 0;
  }
}

long LinuxParser::Jiffies() {
  try {
    vector<string> jiffies = CpuUtilization();
    long total = 0;
    for (string jiff : jiffies) {
      total += std::stol(jiff);
    }

    return total;
  } catch (...) {
    return 0;
  }
}

long LinuxParser::ActiveJiffies(int pid) {
  try {
    string line;
    float cpu_usage = 0.0;

    std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);
    if (file.is_open()) {
      std::getline(file, line);
      std::istringstream linestream(line);

      string token;
      long utime = 0;
      long stime = 0;
      long cutime = 0;
      long cstime = 0;
      long starttime = 0;

      for (int i = 0; i < 22 && linestream >> token; i++) {
        switch (i) {
          case 13:
            utime = std::stol(token);
            break;

          case 14:
            stime = std::stol(token);
            break;

          case 15:
            cutime = std::stol(token);
            break;

          case 16:
            cstime = std::stol(token);
            break;

          case 21:
            starttime = std::stol(token);
            break;
        }
      }

      long total_time = utime + stime + cutime + cstime;

      float seconds = UpTime() - (starttime / sysconf(_SC_CLK_TCK));

      cpu_usage = (total_time / sysconf(_SC_CLK_TCK)) / seconds;
    }

    file.close();

    if (cpu_usage < 0)
      return 0;
    else if (cpu_usage > 1)
      return 1;
    else
      return cpu_usage;

  } catch (...) {
    return 0;
  }
}

long LinuxParser::ActiveJiffies() {
  try {
    vector<string> jiffies = CpuUtilization();
    long total = 0;
    for (int i = 0; i < 8; i++) {
      if (i != kIdle_ && i != kIOwait_) total += std::stol(jiffies[i]);
    }

    return (total);
  } catch (...) {
    return 0;
  }
}

long LinuxParser::IdleJiffies() {
  try {
    vector<string> jiffies = CpuUtilization();
    long idle = std::stol(jiffies[kIdle_]) + std::stol(jiffies[kIOwait_]);

    return idle;
  } catch (...) {
    return 0;
  }
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

  file.close();

  return cpu;
}

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  
  int totalProcesses = 0;

  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream line_s(line);

      while (line_s >> key >> value) {
        if (key == filterProcesses) {
          totalProcesses = std::stoi(value);
        }
      }
    }
  }

  file.close();

  return totalProcesses;
}

int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  
  int runningProcesses = 0;

  std::ifstream file(kProcDirectory + kStatFilename);
  if (file.is_open()) {
    while (std::getline(file, line)) {
      std::istringstream line_s(line);

      while (line_s >> key >> value) {
        if (key == filterRunningProcesses) {
          runningProcesses = std::stoi(value);
        }
      }
    }
  }

  file.close();

  return runningProcesses;
}

string LinuxParser::Command(int pid) {
  try {
    string line;
    string key;
    string value;
    std::ifstream file(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

    std::getline(file, line);

    file.close();

    return line;
  } catch (...) {
    return string();
  }
}

string LinuxParser::Ram(int pid) {
  try {
    string line;
    string key;
    string value;
    std::ifstream file(kProcDirectory + std::to_string(pid) + kStatusFilename);

    while (std::getline(file, line)) {
      std::istringstream linestream(line);

      linestream >> key >> value;

      // filterProcMem = VmData not VmSize ,
      // VmData will show the exact physical memory being used.
      if (key == filterProcMem) {
        value = to_string(std::stol(value) / 1024);
        break;
      }
    }

    file.close();

    return value;
  } catch (...) {
    return string();
  }
}

string LinuxParser::Uid(int pid) {
  try {
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

    file.close();

    return uid;
  } catch (...) {
    return string();
  }
}

string LinuxParser::User(int pid) {
  try {
    string uid = Uid(pid);

    string line;
    string user;

    std::ifstream file(kPasswordPath);
    if (file.is_open()) {
      while (getline(file, line)) {
        std::replace(line.begin(), line.end(), ':', ' ');

        std::istringstream linestream(line);

        string passwd;
        string uid2;
        string gid;
        string comment;
        string home;
        string bash;

        linestream >> user >> passwd >> uid2 >> gid >> comment >> home >> bash;

        if (uid2 == uid) {
          break;
        }
      }
    }

    file.close();

    return user;
  } catch (...) {
    return string();
  }
}

long LinuxParser::UpTime(int pid) {
  try {
    string line;
    long uptime = 0;

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

      file.close();

      return uptime;
    }

    file.close();

    return uptime;
  } catch (...) {
    return 0;
  }
}