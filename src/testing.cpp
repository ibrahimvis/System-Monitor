// #include <dirent.h>
// #include <unistd.h>

// #include <iostream>  // for debugging
// #include <sstream>
// #include <string>
// #include <vector>

// #include "../include/format.h"
// #include "../include/linux_parser.h"
// #include "../include/process.h"
// #include "../include/system.h"

// using std::cout;
// using std::endl;
// using std::stof;
// using std::string;
// using std::to_string;
// using std::vector;

// float LinuxParser::MemoryUtilization() {
//   float shmem = 0.0;
//   float sreclaimable = 0.0;
//   float usedMem = 0.0, cachedMem = 0.0, totalMem = 0.0, freeMem = 0.0;

//   string line;
//   string key;
//   string value;
//   std::ifstream filestream(kProcDirectory + kMeminfoFilename);
//   if (filestream.is_open()) {
//     while (std::getline(filestream, line)) {
//       std::replace(line.begin(), line.end(), ' ', '_');
//       std::replace(line.begin(), line.end(), ':', ' ');

//       std::istringstream linestream(line);
//       while (linestream >> key >> value) {
//         // value.erase(remove(value.begin(), value.end(), 'kB'), value.end());
//         char chars[] = {'_', ' ', 'k', 'B'};
//         for (char c : chars) {
//           value.erase(remove(value.begin(), value.end(), c), value.end());
//         }

//         switch (key[0]) {
//           case 'M':
//             if (key == "MemTotal") {
//               totalMem = stof(value);
//             } else if (key == "MemFree") {
//               freeMem = stof(value);
//             }
//             // else if ((key == "MemShared") { sharedmem }
//             break;
//           case 'C':
//             if (key == "Cached") {
//               cachedMem = stof(value);
//             }
//             break;
//           case 'S':
//             if (key == "Shmem") {
//               shmem = stof(value);
//             }
//             if (key == "SReclaimable") {
//               sreclaimable = stof(value);
//             }
//             break;
//         }
//         //   if (key == "PRETTY_NAME") {
//         //     std::replace(value.begin(), value.end(), '_', ' ');
//         //     std::cout << value << std::endl;
//         // //     return value;
//         //   }
//       }
//     }
//   }

//   usedMem = totalMem - freeMem;
//   cachedMem = cachedMem + sreclaimable - shmem;

//   std::cout << usedMem / 1000 << std::endl;

//   return usedMem + cachedMem;
// }

// int LinuxParser::TotalProcesses() {
//   string line, key, value;
//   float totalProcesses = 0.0;
//   std::ifstream file(kProcDirectory + kStatFilename);
//   if (file.is_open()) {
//     while (std::getline(file, line)) {
//       // line.replace(line.find(" "), line.length(), "_");

//       std::istringstream line_s(line);

//       while (line_s >> key >> value) {
//         if (key == "processes") {
//           totalProcesses = stof(value);
//         }
//       }
//     }
//   }

//   return totalProcesses;
// }

// long LinuxParser::UpTime() {
//   long upTime = 0;
//   long suspendL, idleL;
//   string line, suspend, idle;

//   std::ifstream file(kProcDirectory + kUptimeFilename);
//   if (file.is_open()) {
//     std::getline(file, line);
//     std::istringstream line_s(line);
//     line_s >> suspend >> idle;
//     suspendL = std::stol(suspend);
//     // idleL = std::stol(idle);

//     upTime = suspendL;
//   }

//   return upTime;
// }

// vector<string> LinuxParser::CpuUtilization() {
//   vector<string> cpu{};
//   string line;

//   std::ifstream file(kProcDirectory + kStatFilename);

//   if (file.is_open()) {
//     std::getline(file, line);
//     std::istringstream line_s(line);
//     while (!line_s.eof()) {
//       string token;
//       line_s >> token;
//       cpu.push_back(token);
//     }

//     cpu.erase(cpu.begin());
//   }

//   return cpu;
// }

// string LinuxParser::Uid(int pid) {
//   string line;
//   string uid = "none";

//   std::ifstream file(kProcDirectory + to_string(pid) + kStatusFilename);

//   if (file.is_open()) {
//     while (std::getline(file, line)) {
//       std::istringstream line_s(line);
//       string name, value, extra;

//       line_s >> name >> value >> extra;
//       if (name == "Uid:") {
//         uid = value;
//         break;
//       }
//     }
//   }

//   return uid;
// }

// string LinuxParser::User(int pid) {
//   string uid = Uid(pid);

//   string line;
//   string user;

//   std::ifstream file(kPasswordPath);
//   if (file.is_open()) {
//     while (getline(file, line)) {
//       // std::replace(line.begin(), line.end(), ' ', '_');
//       std::replace(line.begin(), line.end(), ':', ' ');

//       std::istringstream linestream(line);
//       string passwd, uid2, gid, comment, home, bash;
//       linestream >> user >> passwd >> uid2 >> gid >> comment >> home >> bash;

//       if (uid2 == uid) {
//         break;
//       }
//     }
//   }
//   return user;
// }

// long LinuxParser::CpuUtilization(int pid) {
//   string line;
//   long cpu_usage;

//   std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);
//   if (file.is_open()) {
//     std::getline(file, line);
//     std::istringstream linestream(line);

//     string token;
//     int utime, stime, cutime, cstime, starttime;

//     /*

//     #14 utime - CPU time spent in user code, measured in clock ticks
//     #15 stime - CPU time spent in kernel code, measured in clock ticks
//     #16 cutime - Waited-for children's CPU time spent in user code (in clock
//     ticks) #17 cstime - Waited-for children's CPU time spent in kernel code (in
//     clock ticks) #22 starttime - Time when the process started, measured in
//     clock ticks

//     */
//     for (int i = 0; i < 22 && linestream >> token; i++) {
//       switch (i) {
//         case 0:
//           std::cout << i << " " << token << std::endl;
//           break;
//         case 13:
//           utime = std::stoi(token);
//           std::cout << i << " " << token << std::endl;
//           break;
//         case 14:
//           stime = std::stoi(token);
//           std::cout << i << " " << token << std::endl;
//           break;
//         case 15:
//           cutime = std::stoi(token);
//           std::cout << i << " " << token << std::endl;
//           break;
//         case 16:
//           cstime = std::stoi(token);
//           std::cout << i << " " << token << std::endl;
//           break;
//         case 21:
//           starttime = std::stoi(token);
//           std::cout << i << " " << token << std::endl;
//           break;
//       }
//       // std::cout << i << " " << token << std::endl;
//     }

//     double total_time = utime + stime + cutime + cstime;
//     std::cout << "Total: " << total_time << std::endl;
//     double seconds = LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
//     std::cout << "Seconds: " << seconds << std::endl;
//     cpu_usage = 100 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
//     std::cout << "cpu Usage: " << cpu_usage << std::endl;
//   }

//   return cpu_usage;
// }

// string LinuxParser::Ram(int pid) {
//   string line;
//   string key, value;
//   std::ifstream file(kProcDirectory + std::to_string(pid) + kStatusFilename);

//   while (std::getline(file, line)) {
//     std::istringstream linestream(line);

//     linestream >> key >> value;
//     if (key == "VmSize:") {
//       value = to_string(std::stol(value) / 1000);
//       break;
//     }
//   }
//   return value;
// }

// string LinuxParser::Command(int pid) {
//   string line;
//   string key, value;
//   std::ifstream file(kProcDirectory + std::to_string(pid) + kCmdlineFilename);

//   std::getline(file, line);

//   return line;
// }

// long LinuxParser::UpTime(int pid) {
//   string line;
//   long int uptime = 0;

//   std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);
//   if (file.is_open()) {
//     std::getline(file, line);
//     std::istringstream linestream(line);

//     string token;

//     for (int i = 0; i < 22 && linestream >> token; i++) {
//       if (i == 21) {
//         uptime = std::stol(token) / sysconf(_SC_CLK_TCK);
//         break;
//       }
//     }

//     return uptime;
//   }

//   return uptime;
// }

// vector<Process>& System::Processes() {
//   vector<int> pids = LinuxParser::Pids();
//   for (int pid : pids) {
//     Process process =
//         Process(pid, LinuxParser::User(pid), LinuxParser::Command(pid), 0.0,
//                 LinuxParser::Ram(pid), LinuxParser::UpTime(pid));
//     processes_.push_back(process);
//   }

//   return processes_;
// }

// vector<int> LinuxParser::Pids() {
//   vector<int> pids;
//   DIR* directory = opendir(kProcDirectory.c_str());
//   struct dirent* file;
//   while ((file = readdir(directory)) != nullptr) {
//     // Is this a directory?
//     if (file->d_type == DT_DIR) {
//       // Is every character of the name a digit?
//       string filename(file->d_name);
//       if (std::all_of(filename.begin(), filename.end(), isdigit)) {
//         int pid = stoi(filename);
//         pids.push_back(pid);
//       }
//     }
//   }
//   closedir(directory);
//   return pids;
// }

// int Process::Pid() { return pid; }

// // TODO: Return this process's CPU utilization
// float Process::CpuUtilization() { return cpuUtilz; }

// // TODO: Return the command that generated this process
// string Process::Command() { return command; }

// // TODO: Return this process's memory utilization
// string Process::Ram() { return ram; }

// // TODO: Return the user (name) that generated this process
// string Process::User() { return user; }

// // TODO: Return the age of this process (in seconds)
// long int Process::UpTime() { return uptime; }

// long LinuxParser::Jiffies() {
//   vector<string> jiffies = LinuxParser::CpuUtilization();
//   long total = 0;
//   for (string jiff : jiffies) {
//     total += std::stoi(jiff);
//   }

//   return total;
// }

// // TODO: Read and return the number of active jiffies for a PID
// // REMOVE: [[maybe_unused]] once you define the function
// long LinuxParser::ActiveJiffies(int pid) {
//   string line;
//   long cpu_usage;

//   std::ifstream file(kProcDirectory + to_string(pid) + kStatFilename);
//   if (file.is_open()) {
//     std::getline(file, line);
//     std::istringstream linestream(line);

//     string token;
//     int utime, stime, cutime, cstime, starttime;

//     /*

//     #14 utime - CPU time spent in user code, measured in clock ticks
//     #15 stime - CPU time spent in kernel code, measured in clock ticks
//     #16 cutime - Waited-for children's CPU time spent in user code (in clock
//     ticks) #17 cstime - Waited-for children's CPU time spent in kernel code (in
//     clock ticks) #22 starttime - Time when the process started, measured in
//     clock ticks

//     */
//     for (int i = 0; i < 22 && linestream >> token; i++) {
//       switch (i) {
//         case 13:
//           utime = std::stoi(token);
//           //std::cout << i << " " << token << std::endl;
//           break;
//         case 14:
//           stime = std::stoi(token);
//           //std::cout << i << " " << token << std::endl;
//           break;
//         case 15:
//           cutime = std::stoi(token);
//           //std::cout << i << " " << token << std::endl;
//           break;
//         case 16:
//           cstime = std::stoi(token);
//           //std::cout << i << " " << token << std::endl;
//           break;
//         case 21:
//           starttime = std::stoi(token);
//           //std::cout << i << " " << token << std::endl;
//           break;
//       }
//       // std::cout << i << " " << token << std::endl;
//     }

//     double total_time = utime + stime + cutime + cstime;
//     // std::cout << "Total: " << total_time << std::endl;
//     double seconds = LinuxParser::UpTime() - (starttime / sysconf(_SC_CLK_TCK));
//     // std::cout << "Seconds: " << seconds << std::endl;
//     cpu_usage = 100 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
//     // std::cout << "cpu Usage: " << cpu_usage << std::endl;
//   }

//   return cpu_usage;
//   return 0;
// }

// // TODO: Read and return the number of active jiffies for the system
// long LinuxParser::ActiveJiffies() {
//   vector<string> jiffies = LinuxParser::CpuUtilization();
//   long total = 0;
//   for (int i = 0; i < 8; i++) {
//     total += std::stol(jiffies[i]);
//   }

//   return (total - std::stol(jiffies[kIOwait_]) - std::stol(jiffies[kIdle_]));
// }

// // TODO: Read and return the number of idle jiffies for the system
// long LinuxParser::IdleJiffies() {
//   vector<string> jiffies = LinuxParser::CpuUtilization();
//   long idle = std::stol(jiffies[kIdle_]) + std::stol(jiffies[kIOwait_]);

//   return idle;
// }


// int main() {
//   // float x = LinuxParser::MemoryUtilization();
//   // int y = LinuxParser::TotalProcesses();
//   //   long z = LinuxParser::UpTime();
//   //   std::cout << z << std::endl;
//   //   vector<string> temp = LinuxParser::CpuUtilization();
//   //   for (auto x: temp) {
//   //       std::cout << x << std::endl;
//   //   }
//   //   string uid = LinuxParser::User(1764);

//   cout << LinuxParser::User(14966) << endl;
//   //   System s;
//   //   vector<Process> pro = s.Processes();
//   //   for (Process p : pro) {
//   //     cout << p.Pid() << " " << p.User() << " " << p.Command() << endl;
//   //   }
// }