#include "SRTFScheduler.h"
#include <climits>
#include <stdexcept>

void SRTFScheduler::run(std::vector<Process>& processes) {
    ganttChart.clear();
    if (processes.empty()) return;

    for (Process& p : processes) {
        if (p.arrivalTime < 0)
            throw std::invalid_argument("Arrival Time cannot be negative.");
        if (p.burstTime <= 0)
            throw std::invalid_argument("Burst Time must be greater than zero.");
        p.remainingTime = p.burstTime;
        p.completionTime = p.turnaroundTime = p.waitingTime = 0;
    }

    int completed = 0;
    int currentTime = 0;
    const int n = static_cast<int>(processes.size());

    while (completed < n) {
        int selected = -1;
        int minRemaining = INT_MAX;

        // Choose the arrived process with the shortest remaining time.
        for (int i = 0; i < n; ++i) {
            const Process& p = processes[i];
            if (p.arrivalTime <= currentTime &&
                p.remainingTime > 0 &&
                p.remainingTime < minRemaining) {
                minRemaining = p.remainingTime;
                selected = i;
            }
        }

        // CPU is idle until another process arrives.
        if (selected == -1) {
            ++currentTime;
            continue;
        }

        Process& p = processes[selected];
        --p.remainingTime;

        // Merge adjacent time units belonging to the same process.
        if (!ganttChart.empty() &&
            ganttChart.back().processID == p.processID &&
            ganttChart.back().endTime == currentTime) {
            ++ganttChart.back().endTime;
        } else {
            ganttChart.push_back({p.processID, currentTime, currentTime + 1});
        }

        ++currentTime;

        if (p.remainingTime == 0) {
            ++completed;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.turnaroundTime - p.burstTime;
        }
    }
}

double SRTFScheduler::getAverageWaitingTime(const std::vector<Process>& processes) const {
    if (processes.empty()) return 0.0;
    double total = 0.0;
    for (const Process& p : processes) total += p.waitingTime;
    return total / processes.size();
}

double SRTFScheduler::getAverageTurnaroundTime(const std::vector<Process>& processes) const {
    if (processes.empty()) return 0.0;
    double total = 0.0;
    for (const Process& p : processes) total += p.turnaroundTime;
    return total / processes.size();
}

const std::vector<GanttSegment>& SRTFScheduler::getGanttChart() const {
    return ganttChart;
}
