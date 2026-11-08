#include "SRTFScheduler.h"

#include <algorithm>
#include <climits>
#include <stdexcept>

// Ported from the original SRTF_VisualStudio_Project SRTFScheduler::run() —
// shortest-remaining-time selection, preemption, and Gantt-segment merging
// are unchanged. processID is renamed to pid to match core::Process/
// core::GanttSegment; run()/getGanttChart()/getAverage*() are folded into
// the single schedule() call required by SchedulingAlgorithm, with averaging
// delegated to the shared ScheduleResult::computeAverages().
ScheduleResult SRTFScheduler::schedule(const std::vector<Process> &input) const
{
    std::vector<Process> processes = input;

    ScheduleResult result;
    if (processes.empty())
        return result;

    for (Process &p : processes) {
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
            const Process &p = processes[static_cast<size_t>(i)];
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

        Process &p = processes[static_cast<size_t>(selected)];
        --p.remainingTime;

        // Merge adjacent time units belonging to the same process.
        if (!result.ganttChart.empty() &&
            result.ganttChart.back().pid == p.pid &&
            result.ganttChart.back().endTime == currentTime) {
            ++result.ganttChart.back().endTime;
        } else {
            result.ganttChart.push_back({p.pid, currentTime, currentTime + 1});
        }

        ++currentTime;

        if (p.remainingTime == 0) {
            ++completed;
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.turnaroundTime - p.burstTime;
        }
    }

    result.processes = processes;
    std::sort(result.processes.begin(), result.processes.end(),
              [](const Process &a, const Process &b) { return a.pid < b.pid; });

    result.computeAverages();
    return result;
}

std::string SRTFScheduler::name() const
{
    return "SRTF";
}
