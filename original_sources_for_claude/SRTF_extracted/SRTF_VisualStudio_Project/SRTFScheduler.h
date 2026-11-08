#ifndef SRTFSCHEDULER_H
#define SRTFSCHEDULER_H
#include <vector>
#include "Process.h"
#include "GanttChart.h"

class SRTFScheduler {
public:
    void run(std::vector<Process>& processes);
    double getAverageWaitingTime(const std::vector<Process>& processes) const;
    double getAverageTurnaroundTime(const std::vector<Process>& processes) const;
    const std::vector<GanttSegment>& getGanttChart() const;
private:
    std::vector<GanttSegment> ganttChart;
};
#endif
