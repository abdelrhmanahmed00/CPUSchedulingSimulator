#ifndef PROCESS_H
#define PROCESS_H

struct Process {
    int pid = 0;
    int arrivalTime = 0;
    int burstTime = 0;

    int remainingTime = 0;
    int completionTime = 0;
    int turnaroundTime = 0;
    int waitingTime = 0;
};

#endif // PROCESS_H
