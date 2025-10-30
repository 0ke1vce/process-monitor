#include "system_stats.h"
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <cstdio> // For popen
#include <memory> // For std::unique_ptr
#include <array>  // For std::array

// --- Existing Memory Parser ---
void GetMemoryInfo(long& total, long& free) {
    std::ifstream meminfo_file("/proc/meminfo");
    if (meminfo_file.is_open()) {
        std::string line;
        while (std::getline(meminfo_file, line)) {
            std::stringstream ss(line);
            std::string key;
            long value;
            ss >> key >> value;
            if (key == "MemTotal:") total = value;
            else if (key == "MemAvailable:") free = value; // Use MemAvailable for a more realistic "free" value
        }
    }
}

// --- New Disk Stats Parser ---
void GetDiskStats(std::vector<DiskStats>& disks) {
    static std::map<std::string, std::pair<long long, long long>> prev_stats;
    std::map<std::string, std::pair<long long, long long>> curr_stats;
    disks.clear();

    std::ifstream diskstats_file("/proc/diskstats");
    if (diskstats_file.is_open()) {
        std::string line;
        while (std::getline(diskstats_file, line)) {
            std::stringstream ss(line);
            int major, minor;
            std::string device_name;
            long long sectors_read, sectors_written;
            // Fields: 1, 2, 3 (name), 6 (sectors read), 10 (sectors written)
            ss >> major >> minor >> device_name >> sectors_read >> sectors_read >> sectors_read >> sectors_written >> sectors_written >> sectors_written;
            
            // Filter for physical disks (e.g., sda, hda, nvme...)
            if (device_name.rfind("sd", 0) == 0 || device_name.rfind("hd", 0) == 0 || device_name.rfind("nvme", 0) == 0) {
                 curr_stats[device_name] = {sectors_read, sectors_written};
            }
        }
    }

    for(auto const& [dev, stats] : curr_stats) {
        DiskStats d;
        d.name = dev;
        if (prev_stats.count(dev)) {
            long long read_delta = stats.first - prev_stats[dev].first;
            long long write_delta = stats.second - prev_stats[dev].second;
            // Sector size is 512 bytes. Rate is KB/s.
            d.read_rate_kb = (float)(read_delta * 512) / 1024.0f;
            d.write_rate_kb = (float)(write_delta * 512) / 1024.0f;
        }
        disks.push_back(d);
    }
    prev_stats = curr_stats;
}

// --- New Network Stats Parser ---
void GetNetworkStats(std::vector<NetworkStats>& network) {
    static std::map<std::string, std::pair<long long, long long>> prev_stats;
    std::map<std::string, std::pair<long long, long long>> curr_stats;
    network.clear();
    
    std::ifstream netdev_file("/proc/net/dev");
    if(netdev_file.is_open()) {
        std::string line;
        std::getline(netdev_file, line); // Skip header line 1
        std::getline(netdev_file, line); // Skip header line 2
        while(std::getline(netdev_file, line)) {
            std::stringstream ss(line);
            std::string iface_name;
            long long rx_bytes, tx_bytes;
            ss >> iface_name >> rx_bytes; // Read name and received bytes
            // Skip to the transmitted bytes column (9th value)
            for(int i = 0; i < 8; ++i) ss >> tx_bytes;

            if (iface_name.back() == ':') iface_name.pop_back(); // Remove trailing colon
            curr_stats[iface_name] = {rx_bytes, tx_bytes};
        }
    }

    for(auto const& [iface, stats] : curr_stats) {
        NetworkStats n;
        n.interface_name = iface;
        if (prev_stats.count(iface)) {
            long long rx_delta = stats.first - prev_stats[iface].first;
            long long tx_delta = stats.second - prev_stats[iface].second;
            n.rx_rate_kb = (float)rx_delta / 1024.0f;
            n.tx_rate_kb = (float)tx_delta / 1024.0f;
        }
        network.push_back(n);
    }
    prev_stats = curr_stats;
}

// --- New NVIDIA GPU Parser ---
// Helper to run a command and get its output
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        return "popen() failed!";
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

void GetNvidiaGpuStats(std::vector<GpuStats>& gpus) {
    gpus.clear();
    std::string cmd_output = exec("nvidia-smi --query-gpu=index,name,utilization.gpu,memory.used,memory.total --format=csv,noheader,nounits");
    if (cmd_output.find("NVIDIA-SMI has failed") != std::string::npos || cmd_output.empty()) {
        return; // nvidia-smi not available or failed
    }
    std::stringstream ss(cmd_output);
    std::string line;
    while(std::getline(ss, line)) {
        std::stringstream line_ss(line);
        std::string token;
        GpuStats gpu;
        
        std::getline(line_ss, token, ','); gpu.id = std::stoi(token);
        std::getline(line_ss, token, ','); gpu.name = token;
        std::getline(line_ss, token, ','); gpu.utilization = std::stoi(token);
        std::getline(line_ss, token, ','); gpu.mem_used_mb = std::stoi(token);
        std::getline(line_ss, token, ','); gpu.mem_total_mb = std::stoi(token);
        gpus.push_back(gpu);
    }
}
