#include "process_parser.h"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

namespace fs = std::filesystem;

// Helper function to parse a single line from /proc/[pid]/status
std::string GetValueFromStatus(const std::string& line) {
    size_t colon_pos = line.find(":");
    if (colon_pos != std::string::npos) {
        // Trim leading whitespace
        size_t value_start = line.find_first_not_of(" \t", colon_pos + 1);
        if (value_start != std::string::npos)
            return line.substr(value_start);
    }
    return "";
}

long GetSystemUptime() {
    std::ifstream uptime_file("/proc/uptime");
    long uptime = 0;
    if (uptime_file.is_open()) {
        uptime_file >> uptime;
    }
    return uptime;
}

std::vector<ProcessData> GetAllProcesses() {
    std::vector<ProcessData> processes;
    const std::string proc_path = "/proc";

    for (const auto& entry : fs::directory_iterator(proc_path)) {
        if (entry.is_directory()) {
            std::string dir_name = entry.path().filename().string();
            if (std::all_of(dir_name.begin(), dir_name.end(), ::isdigit)) {
                ProcessData p = {}; // Initialize to zero/empty
                p.pid = std::stoi(dir_name);

                // --- Read from /proc/[pid]/status ---
                std::ifstream status_file(entry.path().string() + "/status");
                if (status_file.is_open()) {
                    std::string line;
                    while (std::getline(status_file, line)) {
                        if (line.rfind("Name:", 0) == 0) p.name = GetValueFromStatus(line);
                        else if (line.rfind("State:", 0) == 0) p.state = GetValueFromStatus(line);
                        else if (line.rfind("VmRSS:", 0) == 0) p.memory = std::stol(GetValueFromStatus(line));
                        else if (line.rfind("PPid:", 0) == 0) p.ppid = std::stoi(GetValueFromStatus(line));
                    }
                }

                // --- Read from /proc/[pid]/stat ---
                std::ifstream stat_file(entry.path().string() + "/stat");
                if (stat_file.is_open()) {
                    std::string line;
                    std::getline(stat_file, line);
                    std::stringstream ss(line);
                    std::string token;
                    for (int i = 0; i < 13; ++i) ss >> token; // Skip first 13 fields
                    ss >> p.utime >> p.stime; // Read 14th (utime) and 15th (stime)
                }

                // --- Read from /proc/[pid]/io ---
                std::ifstream io_file(entry.path().string() + "/io");
                if (io_file.is_open()) {
                    std::string line;
                    while(std::getline(io_file, line)) {
                        std::stringstream ss(line);
                        std::string key;
                        ss >> key;
                        if (key == "rchar:") ss >> p.read_bytes;
                        else if (key == "wchar:") ss >> p.write_bytes;
                    }
                }
                processes.push_back(p);
            }
        }
    }
    return processes;



}

// ... existing code at the top ...

// Add this entire new function at the end of the file
void GetSystemCpuTimes(long long& total_time, long long& idle_time) {
    std::ifstream stat_file("/proc/stat");
    if (stat_file.is_open()) {
        std::string line;
        std::getline(stat_file, line);
        std::stringstream ss(line);
        std::string cpu_label;
        long long user, nice, system, idle, iowait, irq, softirq, steal;
        
        ss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
        
        idle_time = idle + iowait;
        total_time = user + nice + system + idle + iowait + irq + softirq + steal;
    }
}
