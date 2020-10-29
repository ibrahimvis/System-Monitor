#include "format.h"

#include <string>

using std::string;


string Format::ElapsedTime(long seconds) {
  int hours = seconds / 3600;
  int remainder = seconds - hours * 3600;
  int mins = remainder / 60;
  remainder = remainder - mins * 60;
  int secs = remainder;

    return string(((hours >= 10) ? std::to_string(hours) : "0" + std::to_string(hours)) + ":" + ((mins >= 10) ? std::to_string(mins) : "0" + std::to_string(mins)) + ":" + ((secs >= 10) ? std::to_string(secs) : "0" + std::to_string(secs)));
}