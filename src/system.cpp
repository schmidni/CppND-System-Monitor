#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

System::System() {
  setKernel();
  setOperatingSystem();
}


// ACCESSORS **********

// PRIVATE SET: operatingSystem_
void System::setOperatingSystem() {
  operatingSystem_ = LinuxParser::OperatingSystem();
}

// PUBLIC GET: operatingSystem_
std::string System::OperatingSystem() { return operatingSystem_; }

// PRIVATE SET: Return the system's kernel identifier
void System::setKernel() { kernel_ = LinuxParser::Kernel(); }

// PUBLIC GET: Return the system's kernel identifier
std::string System::Kernel() { return kernel_; }

// PRIVATE SET: Init the cpu attribute
void System::setCPU() { cpu_ = Processor(); }

// Return the system's CPU
Processor& System::Cpu() { return cpu_; }


// METHODS ****************

// Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }

// Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// Return a container composed of the system's processes
vector<Process>& System::Processes() {
  // Initializing the whole thing on every call to keep it simple
  // could be done by maintaining a list if performance were a problem
  processes_ = {};
  for (int pid : LinuxParser::Pids()) {
    processes_.push_back(Process(pid));
  }
  sort(processes_.begin(), processes_.end());
  reverse(processes_.begin(), processes_.end());
  return processes_;
}
