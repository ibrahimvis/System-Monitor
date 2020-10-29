#ifndef PROCESS_H
#define PROCESS_H

#include <string>

using std::string;


class Process {
 public:
  Process(int pid_, string user_, string command_, double cpuU_, string ram_,
          long uptime_)
      : pid(pid_),
        user(user_),
        command(command_),
        cpuUtilz(cpuU_),
        ram(ram_),
        uptime(uptime_) {}
  int Pid();                               
  string User();                           
  string Command();                        
  double CpuUtilization();                  
  string Ram();                            
  long int UpTime();                       
  bool operator<(Process const& a) const;  
  bool operator==(Process const& a) const;
  bool operator>(Process const& a) const;


 private:
  int pid;
  string user;
  string command;
  double cpuUtilz;
  string ram;
  long int uptime;
};

#endif