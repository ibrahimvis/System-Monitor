#include "processor.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  // Total CPU time since boot =
  // user+nice+system+idle+iowait+irq+softirq+steal

  // Total CPU Idle time since boot = idle + iowait

  // Total CPU usage time since boot = Total CPU time since boot - Total CPU
  // Idle time since boot

  // Total CPU percentage = Total CPU usage time since boot/Total CPU time since
  // boot X 100

  // float total = Idle + nonIdle;
  float CPU_Percentage = (getTotle() - getIdle()) / getTotle();

  // if (CPU_Percentage < 0)
  //   CPU_Percentage =  CPU_Percentage * -1;
  if (CPU_Percentage < 0)
    return 0;
  else
    return CPU_Percentage;
}

void Processor::setTotal(long total_) { total = total_; }
void Processor::setIdle(long idle_) { idle = idle_; }
long Processor::getIdle() const { return idle; }
long Processor::getTotle() const { return total; }