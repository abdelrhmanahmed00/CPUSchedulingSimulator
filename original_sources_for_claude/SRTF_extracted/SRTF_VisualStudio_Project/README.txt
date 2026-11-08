SRTF Scheduler - Visual Studio project

Open SRTF_Console.sln in Visual Studio 2022.
Select Debug | x64, then Build > Build Solution.
Run with Ctrl+F5.

Files:
Process.h             Process data structure
GanttChart.h          Gantt chart segment structure
SRTFScheduler.h       Scheduler declarations
SRTFScheduler.cpp     SRTF algorithm and metric calculations
main.cpp              Console test using the lecture example

Expected:
P1: CT=17, WT=9, TAT=17
P2: CT=5, WT=0, TAT=4
P3: CT=26, WT=15, TAT=24
P4: CT=10, WT=2, TAT=7
Average WT = 6.50
Average TAT = 13.00

This is the standalone C++ logic module. It is not yet connected to Qt.
