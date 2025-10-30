#ifndef PROCESS_PARSER_H
#define PROCESS_PARSER_H

#include <string>
#include <vector>

struct ProcessData {
    int pid;
    int ppid;
    std::string name;
    std::string state;
    long memory;
    long utime;
    long stime;
    float cpu_usage;
    long long read_bytes;
    long long write_bytes;
    float io_read_rate;  // In KB/s
    float io_write_rate; // In KB/s
};

std::vector<ProcessData> GetAllProcesses();
long GetSystemUptime();

// ... existing code ...
std::vector<ProcessData> GetAllProcesses();
long GetSystemUptime();

// Add this new function declaration
void GetSystemCpuTimes(long long& total_time, long long& idle_time);

#endif // PROCESS_PARSER_H
