#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() const { return this->pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const {
  long uptime = this->UpTime();
  long active_jiffies = LinuxParser::ActiveJiffies(this->Pid());

  return float(active_jiffies) / float(uptime);
}

// TODO: Return the command that generated this process
string Process::Command() {
  string command = LinuxParser::Command(this->Pid());
  if (command.length() > command_max_len) {
    command = command.substr(0, command_max_len) + "...";
  }
  return command;
}

// TODO: Return this process's memory utilization
string Process::Ram() {
  int ram_kb = std::stoi(LinuxParser::Ram(this->Pid()));
  return to_string(ram_kb / 1024);
}

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(this->Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(this->Pid()); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() < a.CpuUtilization();
}