#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "Process.h"
#include "SRTFScheduler.h"

int main() {
    std::vector<Process> processes = {
        {1, 0, 8}, {2, 1, 4}, {3, 2, 9}, {4, 3, 5}
    };

    SRTFScheduler scheduler;
    try {
        scheduler.run(processes);
    } catch (const std::exception& e) {
        std::cerr << "Simulation error: " << e.what() << '\\n';
        return 1;
    }

    std::cout << "PID  AT  BT  CT  WT  TAT\\n";
    for (const Process& p : processes) {
        std::cout << "P" << p.processID << "    "
                  << p.arrivalTime << "   " << p.burstTime << "   "
                  << p.completionTime << "   " << p.waitingTime << "   "
                  << p.turnaroundTime << '\\n';
    }

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\\nAverage Waiting Time: "
              << scheduler.getAverageWaitingTime(processes) << '\\n';
    std::cout << "Average Turnaround Time: "
              << scheduler.getAverageTurnaroundTime(processes) << '\\n';

    std::cout << "\\nGantt Chart:\\n";
    for (const GanttSegment& s : scheduler.getGanttChart()) {
        std::cout << "P" << s.processID << " ["
                  << s.startTime << " - " << s.endTime << "]\\n";
    }
    return 0;
}
