#include <string>
#include "format.h"
#include <math.h>   

using std::string;
using std::to_string;

string Format::ElapsedTime(long seconds) { 
  
  int hours = seconds/3600;
  int minutes = (seconds/60) % 60;
  seconds = seconds % 60;
  
  
  return addZero(hours)+":"+addZero(minutes)+":"+addZero(seconds);
}

// check if calculated number is smaller than 10 and return the appropriate
// string
std::string Format::addZero(long time) {
  if (time < 10)
    return "0" + std::to_string(time);
  else
    return std::to_string(time);
}