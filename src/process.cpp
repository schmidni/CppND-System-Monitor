#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_{pid} {
    user_ = LinuxParser::User(pid);
    command_ = LinuxParser::Command(pid);
    Process::CpuUtilization();
};

// Return this process's ID
int Process::Pid() { 
    return pid_; 
}

// Return this process's CPU utilization
// save to private attribute for comparison operator
float Process::CpuUtilization() { 
    cpu_ = LinuxParser::CpuUtilization(pid_);
    return cpu_;
}

// Return the command that generated this process
string Process::Command() { 
    return command_; 
}

// Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser::Ram(pid_); 
}

// Return the user (name) that generated this process
string Process::User() { 
    return user_;
}

// Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime(pid_); 
}

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return (this->cpu_ < a.cpu_); 
}