#ifndef SRTFSCHEDULER_H
#define SRTFSCHEDULER_H

#include "SchedulingAlgorithm.h"

// Preemptive Shortest Remaining Time First.
class SRTFScheduler : public SchedulingAlgorithm
{
public:
    ScheduleResult schedule(const std::vector<Process> &input) const override;
    std::string name() const override;
};

#endif // SRTFSCHEDULER_H
