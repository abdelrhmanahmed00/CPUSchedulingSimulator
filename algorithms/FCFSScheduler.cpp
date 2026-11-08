#include "FCFSScheduler.h"

#include <algorithm>

// Ported from the original FCFS(1).CPP FCFS::calculateFCFS() — scheduling
// logic and WT/CT/TAT computation order unchanged. The original's string
// process name and cin/cout I/O methods are dropped; pid is the identifier
// and Gantt segments are recorded as data instead of printed.
ScheduleResult FCFSScheduler::schedule(const std::vector<Process> &input) const
{
    std::vector<Process> processes = input;

    // Original used std::sort; stable_sort guarantees the same deterministic
    // result when arrival times tie, which plain sort left unspecified.
    std::stable_sort(processes.begin(), processes.end(),
                      [](const Process &a, const Process &b) {
                          return a.arrivalTime < b.arrivalTime;
                      });

    ScheduleResult result;
    int currentTime = 0;

    for (Process &p : processes) {
        // CPU waits for the next process to arrive if it's idle.
        if (currentTime < p.arrivalTime) {
            result.ganttChart.push_back({-1, currentTime, p.arrivalTime});
            currentTime = p.arrivalTime;
        }

        const int startTime = currentTime;

        const int waiting = currentTime - p.arrivalTime;
        const int completion = currentTime + p.burstTime;
        const int turnaround = completion - p.arrivalTime;

        p.waitingTime = waiting;
        p.completionTime = completion;
        p.turnaroundTime = turnaround;
        p.remainingTime = 0;

        currentTime = completion;

        result.ganttChart.push_back({p.pid, startTime, completion});
        result.processes.push_back(p);
    }

    std::sort(result.processes.begin(), result.processes.end(),
              [](const Process &a, const Process &b) { return a.pid < b.pid; });

    result.computeAverages();
    return result;
}

std::string FCFSScheduler::name() const
{
    return "FCFS";
}
