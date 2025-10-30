#include <ncurses.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <algorithm>
#include <map>
#include <signal.h>
#include "process_parser.h"
#include "ui_manager.h"
#include "system_stats.h"

int main() {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(1000);

    // --- Main State Variables ---
    ViewMode current_view = ViewMode::PROCESSES;
    std::string sort_key = "cpu";
    size_t selected_row = 0;
    
    // --- State for Process Calculations ---
    std::vector<ProcessData> previous_processes;
    long long prev_total_time = 0;
    long long prev_idle_time = 0;

    while (true) {
        int ch = getch(); 
        if (ch == 'q') break;

        // --- Global Input Handling ---
        if (ch == 'v') {
            current_view = (current_view == ViewMode::PROCESSES) ? ViewMode::PERFORMANCE : ViewMode::PROCESSES;
        }

        // --- View-Specific Input Handling ---
        if (current_view == ViewMode::PROCESSES && ch != ERR) {
             switch (ch) {
                case KEY_UP: if (selected_row > 0) selected_row--; break;
                case KEY_DOWN: selected_row++; break; // Boundary check later
                case 'p': sort_key = "pid"; break;
                case 'm': sort_key = "memory"; break;
                case 'c': sort_key = "cpu"; break;
                case 'i': sort_key = "io"; break;
                case 'k':
                    if (!previous_processes.empty() && selected_row < previous_processes.size()) {
                        kill(previous_processes[selected_row].pid, SIGTERM);
                    }
                    break;
            }
        }

        // --- Data Gathering and Processing ---
        SystemStats stats = {};
        std::vector<ProcessData> current_processes;

        // Gather system-wide stats for all views
        long long current_total_time, current_idle_time;
        GetSystemCpuTimes(current_total_time, current_idle_time);
        long long total_time_delta = current_total_time - prev_total_time;
        long long total_idle_time_delta = current_idle_time - prev_idle_time;
        if (total_time_delta > 0) {
            stats.cpu_percent = 100.0f * (float)(total_time_delta - total_idle_time_delta) / (float)total_time_delta;
        }
        GetMemoryInfo(stats.mem_total, stats.mem_free);
        stats.mem_used = stats.mem_total - stats.mem_free;
        if (stats.mem_total > 0) {
            stats.mem_percent = 100.0f * (float)stats.mem_used / (float)stats.mem_total;
        }
        
        // Gather view-specific stats
        if (current_view == ViewMode::PROCESSES) {
            current_processes = GetAllProcesses();
            std::map<int, ProcessData> prev_map;
            for (const auto& p : previous_processes) prev_map[p.pid] = p;
            for (auto& p : current_processes) {
                if (prev_map.count(p.pid)) {
                    // Process calculations...
                }
            }
            if (sort_key == "pid") { std::sort(current_processes.begin(), current_processes.end(), [](const auto& a, const auto& b){ return a.pid < b.pid; }); }
            else if (sort_key == "memory") { std::sort(current_processes.begin(), current_processes.end(), [](const auto& a, const auto& b){ return a.memory > b.memory; }); }
            else if (sort_key == "cpu") { std::sort(current_processes.begin(), current_processes.end(), [](const auto& a, const auto& b){ return a.cpu_usage > b.cpu_usage; }); }
            else if (sort_key == "io") { std::sort(current_processes.begin(), current_processes.end(), [](const auto& a, const auto& b){ return (a.io_read_rate + a.io_write_rate) > (b.io_read_rate + b.io_write_rate); }); }
            if (selected_row >= current_processes.size()) {
                selected_row = current_processes.empty() ? 0 : current_processes.size() - 1;
            }
        } else if (current_view == ViewMode::PERFORMANCE) {
            GetDiskStats(stats.disks);
            GetNetworkStats(stats.network);
            GetNvidiaGpuStats(stats.gpus);
        }

        DrawUI(current_view, stats, current_processes, sort_key, selected_row);
        
        if (current_view == ViewMode::PROCESSES) {
            previous_processes = current_processes;
        }
        prev_total_time = current_total_time;
        prev_idle_time = current_idle_time;
    }

    endwin();
    return 0;
}
