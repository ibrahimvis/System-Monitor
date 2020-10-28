#include "system.h"

#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"

using namespace LinuxParser;
using std::set;
using std::size_t;
using std::string;
using std::vector;

bool Compare(Process p1, Process p2) { return p1 < p2; }

// TODO: Return the system's CPU
Processor& System::Cpu() {
  cpu_.setTotal(LinuxParser::ActiveJiffies());
  cpu_.setIdle(LinuxParser::IdleJiffies());
  return cpu_;
}

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  vector<int> pids = LinuxParser::Pids();
  for (int pid : pids) {
    Process process =
        Process(pid, LinuxParser::User(pid), LinuxParser::Command(pid),
                LinuxParser::ActiveJiffies(pid), LinuxParser::Ram(pid),
                LinuxParser::UpTime(pid));
    processes_.push_back(process);
  }
  sort(processes_.begin(), processes_.end(), Compare);
  processes_.erase(unique(processes_.begin(), processes_.end()),
                   processes_.end());
  return processes_;
}

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// TODO: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }