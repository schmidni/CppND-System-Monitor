#include <string>
#include <sstream>
#include "format.h"
#include <iomanip>

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 

    int hours, minutes;
    minutes = seconds / 60;
    hours = minutes / 60;

    // some formatting, width 2, fill with 0
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << hours << ":";
    oss << std::setw(2) << std::setfill('0') << minutes%60 << ":";
    oss << std::setw(2) << std::setfill('0') << seconds%60;

    return oss.str();
}