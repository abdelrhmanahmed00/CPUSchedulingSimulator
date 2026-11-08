#ifndef SCHEDULINGALGORITHM_H
#define SCHEDULINGALGORITHM_H

#include <string>
#include <vector>

#include "core/Process.h"
#include "core/ScheduleResult.h"

class SchedulingAlgorithm
{
public:
    virtual ~SchedulingAlgorithm() = default;

    virtual ScheduleResult schedule(const std::vector<Process> &input) const = 0;
    virtual std::string name() const = 0;
};

#endif // SCHEDULINGALGORITHM_H
