#ifndef GANTTSEGMENT_H
#define GANTTSEGMENT_H

struct GanttSegment {
    int pid = -1; // -1 represents the CPU being idle
    int startTime = 0;
    int endTime = 0;
};

#endif // GANTTSEGMENT_H
