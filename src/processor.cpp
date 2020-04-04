#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  auto cpu = LinuxParser::CpuUtilization();

  // should be just for the very first read, should be solved in the constructor
  if (prevCpu.size() == 0) {
    prevCpu = cpu;
    return 0.0;
  }

  // use arithmetic from here
  // https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
  int Idle = cpu[3] + cpu[4];
  int PrevIdle = prevCpu[3] + prevCpu[4];

  int NonIdle = cpu[0] + cpu[1] + cpu[2] + cpu[5] + cpu[6] + cpu[7];
  int PrevNonIdle = prevCpu[0] + prevCpu[1] + prevCpu[2] + prevCpu[5] +
                    prevCpu[6] + prevCpu[7];

  int Total = Idle + NonIdle;
  int PrevTotal = PrevIdle + PrevNonIdle;

  int totald = Total - PrevTotal;
  int idled = Idle - PrevIdle;

  float CPU_Percentage = float(totald - idled) / float(totald);

  // keep cpu for next read
  prevCpu = cpu;

  return CPU_Percentage;
}