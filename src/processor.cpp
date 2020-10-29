#include "processor.h"


double Processor::Utilization() {

  double cpu;
  double idled  = getIdle() - getPreIdle();
  double totald = getTotle() - getPreTotal(); 

  cpu = (totald - idled) / totald;

  if (cpu < 0) {
    cpu = 0;//getOther() / getOtherTotal();
    return cpu;
  }
  else
    return cpu;
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