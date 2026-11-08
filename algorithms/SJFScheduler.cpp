#include "SJFScheduler.h"

#include <algorithm>

namespace {
// Mirrors the original SJF::isReady(int i, int currentTime).
bool isReady(const std::vector<Process> &processes, const std::vector<bool> &finished,
             int i, int currentTime)
{
    return !finished[static_cast<size_t>(i)] && processes[static_cast<size_t>(i)].arrivalTime <= currentTime;
}
}

// Ported from the original SJF(1).cpp SJF::calculate() — scheduling logic
// (isReady, shortest-burst selection, arrival-time tie-break, idle handling)
// is unchanged. The original's raw new[]/delete[] arrays (process*,
// GanttEntry*, bool*) are replaced with std::vector as instructed.
ScheduleResult SJFScheduler::schedule(const std::vector<Process> &input) const
{
    std::vector<Process> processes = input;
    std::vector<bool> finished(processes.size(), false);

    ScheduleResult result;
    int currentTime = 0;
    int completed = 0;
    const int n = static_cast<int>(processes.size());

    while (completed < n) {
        int shortestIndex = -1;

        for (int i = 0; i < n; ++i) {
            if (isReady(processes, finished, i, currentTime)) {
                if (shortestIndex == -1
                    || processes[static_cast<size_t>(i)].burstTime
                           < processes[static_cast<size_t>(shortestIndex)].burstTime) {
                    shortestIndex = i;
                } else if (processes[static_cast<size_t>(i)].burstTime
                           == processes[static_cast<size_t>(shortestIndex)].burstTime) {
                    if (processes[static_cast<size_t>(i)].arrivalTime
                        < processes[static_cast<size_t>(shortestIndex)].arrivalTime) {
                        shortestIndex = i;
                    }
                }
            }
        }

        if (shortestIndex == -1) {
            ++currentTime;
        } else {
            const int startTime = currentTime;
            currentTime += processes[static_cast<size_t>(shortestIndex)].burstTime;

            Process &p = processes[static_cast<size_t>(shortestIndex)];
            p.completionTime = currentTime;
            p.turnaroundTime = p.completionTime - p.arrivalTime;
            p.waitingTime = p.turnaroundTime - p.burstTime;
            p.remainingTime = 0;

            result.ganttChart.push_back({p.pid, startTime, currentTime});

            finished[static_cast<size_t>(shortestIndex)] = true;
            ++completed;
        }
    }

    for (const Process &p : processes)
        result.processes.push_back(p);

    std::sort(result.processes.begin(), result.processes.end(),
              [](const Process &a, const Process &b) { return a.pid < b.pid; });

    result.computeAverages();
    return result;
}

std::string SJFScheduler::name() const
{
    return "SJF";
}
