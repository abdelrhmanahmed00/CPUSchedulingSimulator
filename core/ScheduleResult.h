#ifndef SCHEDULERESULT_H
#define SCHEDULERESULT_H

#include <vector>

#include "Process.h"
#include "GanttSegment.h"

struct ScheduleResult {
    std::vector<Process> processes;
    std::vector<GanttSegment> ganttChart;

    double avgWaitingTime = 0.0;
    double avgTurnaroundTime = 0.0;

    void computeAverages();
};

#endif // SCHEDULERESULT_H
