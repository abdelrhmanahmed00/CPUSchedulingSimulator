#ifndef FCFSSCHEDULER_H
#define FCFSSCHEDULER_H

#include "SchedulingAlgorithm.h"

class FCFSScheduler : public SchedulingAlgorithm
{
public:
    ScheduleResult schedule(const std::vector<Process> &input) const override;
    std::string name() const override;
};

#endif // FCFSSCHEDULER_H
