#ifndef SJFSCHEDULER_H
#define SJFSCHEDULER_H

#include "SchedulingAlgorithm.h"

// Non-preemptive Shortest Job First.
class SJFScheduler : public SchedulingAlgorithm
{
public:
    ScheduleResult schedule(const std::vector<Process> &input) const override;
    std::string name() const override;
};

#endif // SJFSCHEDULER_H
