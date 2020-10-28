#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp
  void setTotal(long total_);
  void setIdle(long idle_);
  long getIdle() const;
  long getTotle() const;

  // TODO: Declare any necessary private members
 private:
  long total;
  long idle;
};

#endif