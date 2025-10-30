#include "ui_manager.h"
#include <ncurses.h>

// --- Helper Functions and Color Initialization (from before) ---
void DrawMeter(int y, int x, int width, const char* label, float percent);
void InitializeColors();

// --- NEW: The UI for the Performance Tab ---
void DrawPerformanceUI(const SystemStats& stats) {
    int term_width, term_height;
    getmaxyx(stdscr, term_height, term_width);
    int row = 0;

    // --- CPU and Memory ---
    DrawMeter(row++, 1, term_width / 2 - 2, "CPU", stats.cpu_percent);
    DrawMeter(row++, 1, term_width / 2 - 2, "MEM", stats.mem_percent);
    mvprintw(row, 1, "MEM Used: %ld / %ld KB", stats.mem_used, stats.mem_total);
    row += 2;

    // --- GPUs ---
    attron(A_BOLD);
    mvprintw(row++, 0, "GPUs:");
    attroff(A_BOLD);
    if (stats.gpus.empty()) {
        mvprintw(row++, 1, "No NVIDIA GPU detected.");
    } else {
        for (const auto& gpu : stats.gpus) {
            mvprintw(row, 1, "%s (GPU %d)", gpu.name.c_str(), gpu.id);
            DrawMeter(row + 1, 2, term_width - 4, "Util", gpu.utilization);
            mvprintw(row + 2, 2, "VRAM: %d / %d MiB", gpu.mem_used_mb, gpu.mem_total_mb);
            row += 4;
        }
    }

    // --- Disks and Network ---
    int col_width = term_width / 2;
    int disk_row = row;
    int net_row = row;
    attron(A_BOLD);
    mvprintw(disk_row++, 0, "Disks:");
    mvprintw(net_row++, col_width, "Network:");
    attroff(A_BOLD);

    for (const auto& disk : stats.disks) {
        if(disk_row < term_height -1)
            mvprintw(disk_row++, 1, "%-8s R: %7.1f KB/s, W: %7.1f KB/s", disk.name.c_str(), disk.read_rate_kb, disk.write_rate_kb);
    }
    for (const auto& net : stats.network) {
         if(net_row < term_height -1)
            mvprintw(net_row++, col_width + 1, "%-8s RX: %7.1f KB/s, TX: %7.1f KB/s", net.interface_name.c_str(), net.rx_rate_kb, net.tx_rate_kb);
    }
}

// --- RENAMED: The UI for the Processes Tab ---
void DrawProcessesUI(const std::vector<ProcessData>& processes, const std::string& sort_key, size_t selected_row) {
    attron(COLOR_PAIR(1) | A_BOLD);
    mvprintw(3, 0, " Arrows:Select | 'k':Kill | 'p':PID, 'm':Mem, 'c':CPU, 'i':I/O | 'v':Views | Sort: %s", sort_key.c_str());
    attroff(COLOR_PAIR(1) | A_BOLD);

    attron(A_REVERSE);
    mvprintw(4, 0, "PID   NAME            CPU(%%)  MEM(KB)   READ/s(KB) WRITE/s(KB) STATE      ");
    attroff(A_REVERSE);

    int row_offset = 5;
    for (size_t i = 0; i < processes.size(); ++i) {
        int current_row = i + row_offset;
        if(current_row >= LINES - 1) break;
        const auto& p = processes[i];
        if (i == selected_row) attron(A_REVERSE);
        if (p.state.find("R (running)") != std::string::npos && i != selected_row) attron(COLOR_PAIR(4) | A_BOLD);
        else if (i != selected_row) attron(COLOR_PAIR(3));
        
        mvprintw(current_row, 0, "%-5d %-15.15s %-7.2f %-9ld %-10.1f %-11.1f %-10.10s", 
                 p.pid, p.name.c_str(), p.cpu_usage, p.memory, 
                 p.io_read_rate, p.io_write_rate, p.state.c_str());
        
        if (i == selected_row) attroff(A_REVERSE);
        else { attroff(COLOR_PAIR(3)); attroff(COLOR_PAIR(4) | A_BOLD); }
    }
}

// --- Main DrawUI function that switches between views ---
void DrawUI(enum ViewMode view, const SystemStats& stats, const std::vector<ProcessData>& processes, const std::string& sort_key, size_t selected_row) {
    static bool colors_initialized = false;
    if (!colors_initialized) {
        InitializeColors();
        colors_initialized = true;
    }
    
    clear();
    
    // Main header
    attron(A_BOLD);
    mvprintw(0, 0, "Process Monitor - Press 'v' to switch views");
    attroff(A_BOLD);

    if (view == ViewMode::PERFORMANCE) {
        DrawPerformanceUI(stats);
    } else {
        DrawProcessesUI(processes, sort_key, selected_row);
    }
    
    refresh();
}

// --- Unchanged helper functions ---
void DrawMeter(int y, int x, int width, const char* label, float percent) {
    int bar_width = width - 10;
    if (bar_width < 0) bar_width = 0;
    int fill_width = (int)(bar_width * (percent / 100.0f));
    
    mvprintw(y, x, "%-4s[", label);
    attron(COLOR_PAIR(2));
    for (int i = 0; i < bar_width; ++i) {
        if (i < fill_width) printw("|");
        else printw(" ");
    }
    attroff(COLOR_PAIR(2));
    printw("] %.1f%%", percent);
}

void InitializeColors() {
    start_color();
    init_pair(1, COLOR_CYAN, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_WHITE, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
}
