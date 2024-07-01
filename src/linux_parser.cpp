#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <numeric>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

using std::map;
using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string os_key{"PRETTY_NAME"};
  ReplacementVector replacements, replace_back;

  replacements.push_back(std::make_pair(' ', '_'));
  replacements.push_back(std::make_pair('=', ' '));
  replacements.push_back(std::make_pair('"', ' '));

  replace_back.push_back(std::make_pair('_', ' '));

  return LinuxParser::FindKeyInFile(kOSPath, os_key, replacements,
                                    replace_back);
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return LinuxParser::GetLineElementAtIndex(line, 2);
  }

  return string();
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  const string mem_path = kProcDirectory + kMeminfoFilename;
  string total_key{"MemTotal"}, free_key{"MemFree"};
  ReplacementVector replacements, replace_back;
  replacements.push_back(std::make_pair(':', ' '));
  float mem_total = stof(LinuxParser::FindKeyInFile(
      mem_path, total_key, replacements, replace_back));
  float mem_free = stof(LinuxParser::FindKeyInFile(mem_path, free_key,
                                                   replacements, replace_back));

  return 1.f - mem_free / mem_total;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return stol(LinuxParser::GetLineElementAtIndex(line, 0));
  }

  return 0L;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  long jiff_sum{0};
  for (auto item : jiffies) {
    jiff_sum += stol(item);
  }
  return jiff_sum;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  string line;
  map<int, string> jiffies;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    jiffies = LinuxParser::GetLineElementsAtIndexes(line, {13, 14, 15, 16}, 0);
  }
  long jiff_sum{0};
  for (auto pair : jiffies) {
    jiff_sum += std::stol(pair.second);
  }

  return jiff_sum / sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();

  return stol(jiffies[kUser_]) + stol(jiffies[kNice_]) +
         stol(jiffies[kSystem_]) + stol(jiffies[kIRQ_]) +
         stol(jiffies[kSoftIRQ_]) + stol(jiffies[kSteal_]) +
         stol(jiffies[kGuest_]) + stol(jiffies[kGuestNice_]);
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> jiffies = LinuxParser::CpuUtilization();
  return stol(jiffies[kIdle_]) + stol(jiffies[kIOwait_]);
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<int> cpu_state_indexes{kUser_,   kNice_,     kSystem_,  kIdle_,
                                kIOwait_, kIRQ_,      kSoftIRQ_, kSteal_,
                                kGuest_,  kGuestNice_};

  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  map<int, string> result_map;
  if (stream.is_open()) {
    std::getline(stream, line);
    result_map =
        LinuxParser::GetLineElementsAtIndexes(line, cpu_state_indexes, 1);
  }

  vector<string> result;

  for (auto pair : result_map) {
    result.push_back(pair.second);
  }

  return result;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key{"processes"};
  ReplacementVector rep, rep_back;
  return stoi(LinuxParser::FindKeyInFile(kProcDirectory + kStatFilename, key,
                                         rep, rep_back));
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key{"procs_running"};
  ReplacementVector rep, rep_back;
  return stoi(LinuxParser::FindKeyInFile(kProcDirectory + kStatFilename, key,
                                         rep, rep_back));
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    return LinuxParser::GetLineElementAtIndex(line, 0);
  }

  return string();
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  const string path = kProcDirectory + to_string(pid) + kStatusFilename;
  ReplacementVector replacements{std::make_pair(':', ' ')}, replace_back;
  string mem{"VmSize"};
  return LinuxParser::FindKeyInFile(path, mem, replacements, replace_back);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string key{"Uid"};
  ReplacementVector replacements{std::make_pair(':', ' ')}, replace_back;
  return LinuxParser::FindKeyInFile(
      kProcDirectory + to_string(pid) + kStatusFilename, key, replacements,
      replace_back);
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string line, find_uid = LinuxParser::Uid(pid);
  map<int, string> line_elements;
  std::vector<int> search_elements{0, 2};
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      line_elements =
          LinuxParser::GetLineElementsAtIndexes(line, search_elements, 0);
      if (line_elements[2] == find_uid) return line_elements[0];
    }
  }

  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    const int proc_uptime_pos = 21;
    return std::stol(LinuxParser::GetLineElementAtIndex(line, proc_uptime_pos));
  }

  return 0L;
}

string LinuxParser::GetLineElementAtIndex(const string line, const int n) {
  std::istringstream linestream(line);
  string result;
  int i = 0;
  while (linestream >> result) {
    if (i == n) return result;
    i++;
  }

  return string();
}

map<int, string> LinuxParser::GetLineElementsAtIndexes(
    const std::string line, const vector<int> indexes, const int skip_first) {
  std::istringstream linestream(line);
  map<int, string> result;
  string element;
  int i = 0;
  while (linestream >> element) {
    int curr_ind = i - skip_first;
    if (std::find(indexes.begin(), indexes.end(), curr_ind) != indexes.end()) {
      result[curr_ind] = element;
    }
    i++;
  }

  return result;
}

string LinuxParser::FindKeyInFile(const string path, string key,
                                  ReplacementVector replace_symbols,
                                  ReplacementVector replace_back) {
  string line, curr_key, value;
  std::ifstream filestream(path);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      for (auto pair : replace_symbols) {
        std::replace(line.begin(), line.end(), pair.first, pair.second);
      }
      std::istringstream linestream(line);
      linestream >> curr_key >> value;
      if (curr_key == key) {
        for (auto pair : replace_back) {
          std::replace(value.begin(), value.end(), pair.first, pair.second);
        }
        return value;
      }
    }
  }

  return string();
}
