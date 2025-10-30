#ifndef SYSTEM_STATS_H
#define SYSTEM_STATS_H

#include <string>
#include <vector>

struct DiskStats {
    std::string name;
    float read_rate_kb;
    float write_rate_kb;
};

struct NetworkStats {
    std::string interface_name;
    float rx_rate_kb; // Received
    float tx_rate_kb; // Transmitted
};

struct GpuStats {
    int id;
    std::string name = "N/A";
    int utilization = 0;
    int mem_used_mb = 0;
    int mem_total_mb = 0;
};

struct SystemStats {
    float cpu_percent;
    long mem_total;
    long mem_free;
    long mem_used;
    float mem_percent;
    std::vector<DiskStats> disks;
    std::vector<NetworkStats> network;
    std::vector<GpuStats> gpus;
};

// Existing function
void GetMemoryInfo(long& total, long& free);

// New parsing functions
void GetDiskStats(std::vector<DiskStats>& disks);
void GetNetworkStats(std::vector<NetworkStats>& network);
void GetNvidiaGpuStats(std::vector<GpuStats>& gpus);

#endif // SYSTEM_STATS_H

