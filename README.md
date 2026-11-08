# CPU Scheduling Simulator

A desktop app for simulating and visualizing classic CPU scheduling algorithms, built with **C++17** and **Qt 6 Widgets**. You add processes with their arrival and burst times, pick an algorithm, and run the simulation. The app shows an animated Gantt chart, a per-process results table, and summary statistics.

## Features

- **Process management**: add, edit, and delete processes (PID, arrival time, burst time), with input checks (no negative arrival times, burst time > 0, unique PIDs).
- **Three scheduling algorithms** to choose from in a drop-down.
- **Animated Gantt chart** with Play / Pause / Reset controls and playback speeds of 0.5x, 1x, 2x, and 4x. Idle CPU time is shown as its own segment.
- **Results table** showing each process's completion, turnaround, and waiting times.
- **Summary cards**: Total Processes, Average Waiting Time, Average Turnaround Time, and Total Execution Time.
- **Modern UI** with a custom theme, sidebar navigation, and styled dialogs.

## Implemented Scheduling Algorithms

| Algorithm | Type | Description |
|-----------|------|-------------|
| **FCFS** (First Come, First Served) | Non-preemptive | Runs processes in order of arrival. |
| **SJF** (Shortest Job First) | Non-preemptive | Of the processes that have arrived, runs the one with the shortest burst time. Ties go to the earlier arrival. |
| **SRTF** (Shortest Remaining Time First) | Preemptive | At every time unit, runs the process with the least remaining time. A newly arrived shorter job preempts the running one. |

All algorithms share the `SchedulingAlgorithm` interface (`schedule()` and `name()`), so adding a new algorithm is straightforward.

## Technologies Used

- **C++17**
- **Qt 6** (Core, Widgets), version 6.5 or newer
- **CMake** 3.19 or newer

## Project Structure

```
CPUSchedulingSimulator/
├── CMakeLists.txt              # CMake build configuration
├── main.cpp                    # Application entry point
├── core/                       # Data models
│   ├── Process.h               # Process data (arrival, burst, results)
│   ├── GanttSegment.h          # One Gantt chart segment (pid -1 = idle)
│   ├── ScheduleResult.h/.cpp   # Scheduling output + average calculations
├── algorithms/                 # Scheduling logic
│   ├── SchedulingAlgorithm.h   # Abstract base interface
│   ├── FCFSScheduler.h/.cpp
│   ├── SJFScheduler.h/.cpp
│   └── SRTFScheduler.h/.cpp
├── gui/                        # Qt Widgets user interface
│   ├── mainwindow.h/.cpp       # Main window, process table, controls
│   ├── Sidebar.h/.cpp          # Navigation sidebar
│   ├── StatCard.h/.cpp         # Summary statistic cards
│   ├── GanttChartWidget.h/.cpp # Custom-painted animated Gantt chart
│   ├── GanttChartPanel.h/.cpp  # Gantt chart + playback controls
│   ├── ProcessDialog.h/.cpp    # Add / edit process dialog
│   └── Theme.h/.cpp            # Colors and stylesheet
└── original_sources_for_claude/ # Original standalone console versions
    ├── FCFS(1).CPP               (reference for the ported algorithms)
    ├── SJF(1).cpp
    ├── SRTF_VisualStudio_Project(2).zip
    └── SRTF_extracted/
```

## Build and Run

### Prerequisites

- Qt 6.5+ (installed with the Qt Online Installer or Homebrew: `brew install qt`)
- CMake 3.19+
- A C++17 compiler (Clang/Xcode Command Line Tools on macOS, GCC, or MSVC)

### Option 1: Qt Creator

1. Open Qt Creator and choose **File → Open File or Project…**
2. Select `CMakeLists.txt` in the project root.
3. Pick a Qt 6 kit (for example, *Desktop Qt 6.x for macOS*) and click **Configure Project**.
4. Press **Ctrl+R** (**⌘R** on macOS) to build and run.

### Option 2: CMake from the command line

```bash
git clone https://github.com/abdelrhmanahmed00/CPUSchedulingSimulator.git
cd CPUSchedulingSimulator

# Configure. Set CMAKE_PREFIX_PATH if CMake cannot find Qt, e.g.
#   -DCMAKE_PREFIX_PATH=$(brew --prefix qt)
#   -DCMAKE_PREFIX_PATH=~/Qt/6.x.x/macos
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build

# Run
open build/CPUSchedulingSimulator.app        # macOS
./build/CPUSchedulingSimulator               # Linux
build\Release\CPUSchedulingSimulator.exe     # Windows
```

## Understanding the Results

For each process, the simulator computes:

| Metric | Formula | Meaning |
|--------|---------|---------|
| **Completion Time (CT)** | time the process finishes | When the process's last burst ends. |
| **Turnaround Time (TAT)** | `CT − Arrival Time` | Total time the process spent in the system. |
| **Waiting Time (WT)** | `TAT − Burst Time` | Time spent in the ready queue, not running. |

Summary statistics:

- **Average Waiting Time**: mean WT across all processes. Lower means processes wait less on average.
- **Average Turnaround Time**: mean TAT across all processes.
- **Total Execution Time**: the time at which the last process finishes (the end of the Gantt chart), including any idle gaps.

The **Gantt chart** shows which process held the CPU over each time interval, and it marks idle periods when no process had arrived yet. Running the same workload under each algorithm shows how they compare. SJF and SRTF usually lower the average waiting time compared with FCFS. SRTF, being preemptive, often gives the lowest average waiting time, at the cost of more context switches.

### Example

The included SRTF reference example (P1–P4) produces **Average WT = 6.50** and **Average TAT = 13.00**.
