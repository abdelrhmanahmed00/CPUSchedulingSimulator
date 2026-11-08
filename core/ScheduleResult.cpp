#include "ScheduleResult.h"

void ScheduleResult::computeAverages()
{
    if (processes.empty()) {
        avgWaitingTime = 0.0;
        avgTurnaroundTime = 0.0;
        return;
    }

    double totalWaiting = 0.0;
    double totalTurnaround = 0.0;

    for (const Process &p : processes) {
        totalWaiting += p.waitingTime;
        totalTurnaround += p.turnaroundTime;
    }

    avgWaitingTime = totalWaiting / static_cast<double>(processes.size());
    avgTurnaroundTime = totalTurnaround / static_cast<double>(processes.size());
}
