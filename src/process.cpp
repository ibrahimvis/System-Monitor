#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

using std::string;
using std::to_string;
using std::vector;

int Process::Pid() { return pid; }

double Process::CpuUtilization() { return cpuUtilz; }

string Process::Command() {
  if (command.size() > 40) {
    command = command.substr(0, 40) + "...";
  }
  return command;
}

string Process::Ram() { return ram; }

string Process::User() { return user; }

long int Process::UpTime() { return uptime; }

bool Process::operator<(Process const& a) const {
  return cpuUtilz < a.cpuUtilz;
}

bool Process::operator>(Process const& a) const {
  return cpuUtilz > a.cpuUtilz;
}