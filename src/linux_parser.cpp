#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> version >> os >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Helper: get value by key in easy to read files
std::string LinuxParser::GetValueFromKey(std::string filename,
                                         std::string key) {
  std::string value, key_, line;
  std::ifstream stream(filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key_ >> value;
      if (key_ == key) {
        return value;
      }
    }
  }
  return "-1";
}

// Helper: return nth word of a string
std::string LinuxParser::GetNthWord(std::istringstream &linestream, std::size_t n) {
  string s;
  while (n-- > 0 && (linestream >> s))
    ;
  return s;
}

// Read and return the system memory utilization (MemTotal-Memfree)/MemTotal
float LinuxParser::MemoryUtilization() {
  string filename = kProcDirectory + kMeminfoFilename;
  // to avoid 2 separate reads, GetValueFromKey could be updated to take and
  // return a vector of strings
  long memTotal =
      std::stol(LinuxParser::GetValueFromKey(filename, "MemTotal:"));
  long memFree = std::stol(LinuxParser::GetValueFromKey(filename, "MemFree:"));
  return float(memTotal - memFree) / float(memTotal);
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  long upTime, idle;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime >> idle;
  }
  return upTime;
}

// Read and return CPU utilization
vector<int> LinuxParser::CpuUtilization() {
  string line, key, value;
  vector<int> cpu;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> key;
    while (linestream >> value) {
      cpu.push_back(std::stoi(value));
    }
  }
  return cpu;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string filename = kProcDirectory + kStatFilename;
  return std::stoi(LinuxParser::GetValueFromKey(filename, "processes"));
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string filename = kProcDirectory + kStatFilename;
  return std::stoi(LinuxParser::GetValueFromKey(filename, "procs_running"));
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  // get command by pid
  string line;
  string filename =
      kProcDirectory + "/" + std::to_string(pid) + kCmdlineFilename;
  std::ifstream stream(filename);
  if (stream.is_open()) std::getline(stream, line);
  return line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string filename =
      kProcDirectory + "/" + std::to_string(pid) + kStatusFilename;
  long kB = std::stoi(LinuxParser::GetValueFromKey(filename, "VmSize:"));
  return std::to_string(kB / 1000);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  // get UserID by pid
  string filename =
      kProcDirectory + "/" + std::to_string(pid) + kStatusFilename;
  return LinuxParser::GetValueFromKey(filename, "Uid:");
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  // get username by UserID
  string username, x, line;
  string id, uid;
  uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> username >> x >> id;
      if (id == uid) return username;
    }
  }
  return string();
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  long ticks, uptime;
  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) +
                       kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    ticks = std::stol(GetNthWord(linestream, 22));
  }
  uptime = ticks / sysconf(_SC_CLK_TCK);
  uptime = UpTime() - uptime;
  return uptime;
}

// Read and return the CPU utilization of a process
float LinuxParser::CpuUtilization(int pid) {
  int utime, stime, cutime, cstime, starttime;
  string line;

  std::ifstream stream(kProcDirectory + "/" + std::to_string(pid) +
                       kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);

    // drop 13 words from the stream
    GetNthWord(linestream, 13);
    // read cpu stats
    linestream >> utime >> stime >> cutime >> cstime;
    // drop 4 more words
    GetNthWord(linestream, 4);
    // next one is starttime
    linestream >> starttime;
  }

  float total_time = utime + stime + cutime + cstime;
  float seconds = float(UpTime()) - (starttime / float(sysconf(_SC_CLK_TCK)));

  return total_time / float(sysconf(_SC_CLK_TCK)) / seconds;
}