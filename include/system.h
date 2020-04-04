#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>

#include "process.h"
#include "processor.h"

class System {
 public:
  System();  // System Constructor

  Processor& Cpu();  // TODO: See src/system.cpp
  std::vector<Process>& Processes();
  float MemoryUtilization();
  long UpTime();
  int TotalProcesses();
  int RunningProcesses();
  std::string Kernel();
  std::string OperatingSystem();

  // Define any necessary private members
 private:
  // set methods
  void setKernel();
  void setOperatingSystem();
  void setCPU();

  // private attributes
  Processor cpu_ = {};
  std::vector<Process> processes_ = {};
  std::string kernel_ = "";
  std::string operatingSystem_ = "";
};

#endif