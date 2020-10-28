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

  /*
  PrevIdle = previdle + previowait
Idle = idle + iowait

PrevNonIdle = prevuser + prevnice + prevsystem + previrq + prevsoftirq + prevsteal
NonIdle = user + nice + system + irq + softirq + steal

PrevTotal = PrevIdle + PrevNonIdle
Total = Idle + NonIdle

# differentiate: actual value minus the previous one
totald = Total - PrevTotal
idled = Idle - PrevIdle

CPU_Percentage = (totald - idled)/totald
  */

  // float cpu = (getTotle() - getIdle()) / getTotle();
  double cpu;
  double idled  = getIdle() - getPreIdle();
  double totald = getTotle() - getPreTotal(); 

  cpu = (totald - idled) / totald;
  // if (CPU_Percentage < 0)
  //   CPU_Percentage =  CPU_Percentage * -1;
  if (cpu < 0)
    return (float)(getOther())/getOtherTotal();
  else
    return (float) cpu;
}

void Processor::setTotal(long total_) { total = total_; }
void Processor::setIdle(long idle_) { idle = idle_; }
long Processor::getIdle() const { return idle; }
long Processor::getTotle() const { return total; }

void Processor::setPreTotal(long pre) { preTotal = pre; }
void Processor::setPreIdle(long pre) { preIdle = pre; }
long Processor::getPreIdle() const { return preIdle; }
long Processor::getPreTotal() const { return preTotal; }

void Processor::setOther(long other) { otherWay = other; }
long Processor::getOther() const { return otherWay; }

void Processor::setOtherTotal(long other) { otherTotal = other; }
long Processor::getOtherTotal() const { return otherTotal; }