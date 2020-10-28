#ifndef PROCESS_H
#define PROCESS_H

#include <string>

using std::string;

/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int pid_, string user_, string command_, float cpuU_, string ram_,
          long uptime_)
      : pid(pid_),
        user(user_),
        command(command_),
        cpuUtilz(cpuU_),
        ram(ram_),
        uptime(uptime_) {}
  int Pid();                               // TODO: See src/process.cpp
  string User();                           // TODO: See src/process.cpp
  string Command();                        // TODO: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  string Ram();                            // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp
  bool operator==(Process const& a) const;
  // TODO: Declare any necessary private members
 private:
  int pid;
  string user;
  string command;
  float cpuUtilz;
  string ram;
  long int uptime;
};

#endif