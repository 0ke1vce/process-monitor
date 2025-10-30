# 🖥️ Process Monitor

A lightweight Process Monitoring System built in C++ for the Operating Systems (OS) Project-Based Learning module.
This tool displays key system and process information such as process IDs, CPU usage, memory usage, and process states — providing insight into how process management works at the OS level.

🚀 Features

📊 Real-time Process Monitoring – Displays running processes dynamically.

⚙️ CPU & Memory Tracking – Monitors usage statistics for each process.

🧩 Process Information Table – Organized display using ncurses for a terminal UI.

🧵 Multithreaded Structure (optional) – Efficiently handles updates and refreshes.

🧠 Educational Purpose – Ideal for OS students to learn process management concepts.

🧩 Project Structure
process-monitor/
├── include/            # Header files
├── src/                # Source files (.cpp)
├── build/              # Output binaries (created after compilation)
├── Makefile            # Build automation script
└── README.md           # Project documentation

⚙️ Requirements

Operating System: Linux

Compiler: g++ (supporting C++17 or higher)

Libraries:

ncurses (for terminal UI)

Install dependencies on Ubuntu/Debian:

sudo apt update
sudo apt install g++ make libncurses5-dev libncursesw5-dev

🧰 Build Instructions

Clone this repository:

git clone https://github.com/0ke1vce/process-monitor.git
cd process-monitor


Compile using the included Makefile:

make


This will create the executable inside the build/ directory.

Run the program:

./build/process-monitor


Clean build files:

make clean

🖼️ Preview

(You can insert a screenshot or GIF of the program running here if available)

Example terminal output:

PID   | Process Name       | CPU%  | MEM%  | State
---------------------------------------------------
1234  | firefox            | 12.5  | 4.2   | Running
2441  | gnome-terminal     |  3.0  | 1.1   | Sleeping
...

🧪 Learning Objectives

Understand process states, scheduling, and system resources.

Learn how to use system calls for fetching process info.

Explore C++17 filesystem and ncurses for terminal-based UI.

Strengthen understanding of real-time process updates.

🧱 Possible Extensions

Add process scheduling algorithms (FCFS, SJF, Round Robin, Priority).

Include graphical visualization of CPU usage.

Implement logging to store system snapshots.

Extend to a client-server model for remote monitoring.

👨‍💻 Contributors

Project Author: 0ke1vce

Academic Use: Operating System PBL Project

Language: C++17

Interface: ncurses (CLI-based)

📜 License

This project is open-source and free to use for educational purposes.
Please credit the original author when reusing or modifying this code.

🧠 “Monitoring processes is the first step to understanding how an OS breathes.”

sudo apt update
sudo apt install g++ make libncurses5-dev libncursesw5-dev
