#ifndef PROCESS_H
#define PROCESS_H
struct Process {
    int processID;
    int arrivalTime;
    int burstTime;
    int remainingTime = 0;
    int completionTime = 0;
    int turnaroundTime = 0;
    int waitingTime = 0;
};
#endif
