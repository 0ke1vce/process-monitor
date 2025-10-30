#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "process_parser.h"
#include "system_stats.h"
#include <vector>
#include <string>

// Enum to manage which view is active
enum class ViewMode { PROCESSES, PERFORMANCE };

// The main draw function now takes the view mode
void DrawUI(enum ViewMode view, const SystemStats& stats, const std::vector<ProcessData>& processes, const std::string& sort_key, size_t selected_row);

#endif // UI_MANAGER_H
