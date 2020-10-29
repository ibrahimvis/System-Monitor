#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  double Utilization(); 
  
  void setTotal(long total_);
  void setIdle(long idle_);
  long getIdle() const;
  long getTotle() const;

  void setPreTotal(long pre);
  void setPreIdle(long pre);
  long getPreTotal() const;
  long getPreIdle() const;

  void setOther(long other);
  long getOther() const;
  void setOtherTotal(long other);
  long getOtherTotal() const;

 private:
  long otherWay;
  long otherTotal;  
  long preTotal;
  long preIdle;
  long total;
  long idle;
};

#endif