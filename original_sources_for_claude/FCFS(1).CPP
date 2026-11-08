#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <string>

using namespace std;
// Processes Class
class Process {
private:
    string processName;
    int arrivalTime;
    int burstTime;
    int waitingTime;
    int turnaroundTime;
    int completionTime;

public:
// Constructor initializes process information
    Process(string name, int arrival, int burst)
    {
        processName = name;
        arrivalTime = arrival;
        burstTime = burst;
        waitingTime = 0;
        turnaroundTime = 0;
        completionTime = 0;
    }

    // Getters
    string getName() const { return processName; }
    int getArrivalTime() const { return arrivalTime; }
    int getBurstTime() const { return burstTime; }
    int getWaitingTime() const { return waitingTime; }
    int getTurnaroundTime() const { return turnaroundTime; }
    int getCompletionTime() const { return completionTime; }

    // Setters
    void setWaitingTime(int WT) { waitingTime = WT; }
    void setTurnaroundTime(int TAT) { turnaroundTime = TAT; }
    void setCompletionTime(int CT) { completionTime = CT; }
};
// FCFS Scheduling Class
class FCFS {
private:
    vector<Process> processes; //stores the processes

public: 
    bool inputProcesses() //Reading ande Validating all processes info
    {
        int n;

        cout << "Enter number of processes: ";
        if (!(cin >> n) || n <= 0)
        {
            cerr << "Error: enter a positive number of processes.\n";
            return false;
        }
// Inputing and Validating each proccess's info
        for (int i = 0; i < n; i++)
        {
            string name;
            int arrival, burst;

            cout << "\n Process " << i + 1 << endl;

            cout << "Name: ";
            if (!(cin >> name))
            {
                cerr << "Error: could not read the process name.\n";
                return false; //cerr makes it clearer that this is an error message
            }

            cout << "Arrival Time: ";
            if (!(cin >> arrival) || arrival < 0)
            {
                cerr << "Error: arrival time must be a non-negative integer.\n";
                return false;
            }

            cout << "Burst Time: ";
            if (!(cin >> burst) || burst <= 0)
            {
                cerr << "Error: burst time must be a positive integer.\n";
                return false;
                
            }
// Adds the Proccess to the vector aka the list of processes
            processes.push_back(Process(name, arrival, burst));
        }

        return true;
    }
// A function that calculates the waiting time, turnaround time, and completion time for each process based on the FCFS scheduling algorithm
    void calculateFCFS()
    {
        sort(processes.begin(), processes.end(),
             [](const Process& a, const Process& b)
             {
                // Sorts out the proccesses based on their arrival time
                 return a.getArrivalTime() < b.getArrivalTime();
             });

        int currentTime = 0;

        for (auto& process : processes)
        {
            if (currentTime < process.getArrivalTime()) // CPU waits for the next proccess with an arrival time less than the current time to arrive
            {
                currentTime = process.getArrivalTime();
            }
// Waiting Time, Completion Time, and Turnaround Time calculations
            int waiting = currentTime - process.getArrivalTime();
            int completion = currentTime + process.getBurstTime();
            int turnaround = completion - process.getArrivalTime();
// Stores the calculated values
            process.setWaitingTime(waiting);
            process.setCompletionTime(completion);
            process.setTurnaroundTime(turnaround);

            currentTime = completion; // Updates CPU time
        }
    }

    void displayResults() // Displaying all the FCFS results in a table form
    {
        double totalWT = 0;
        double totalTAT = 0;
        cout << endl;
        cout << "FCFS Scheduling Results\n";
        cout << endl;
       
        cout << left
             << setw(10) << "Process" // setw(10) is used to set the width between the columns in the table to make sure they're equal
             << setw(10) << "AT"
             << setw(10) << "BT"
             << setw(10) << "WT"
             << setw(10) << "TAT"
             << setw(10) << "CT"
             << endl;

        for (const auto& p : processes)
        {
            //Displaying each proccess's details in the table as well
            cout << left
                 << setw(12) << p.getName()
                 << setw(12) << p.getArrivalTime()
                 << setw(12) << p.getBurstTime()
                 << setw(12) << p.getWaitingTime()
                 << setw(12) << p.getTurnaroundTime()
                 << setw(12) << p.getCompletionTime()
                 << endl;
                 //Claculating both AWT and ATAT as well as displaying them
            totalWT += p.getWaitingTime();
            totalTAT += p.getTurnaroundTime();
        }
        cout << fixed << setprecision(2);
        cout << "\n Average Waiting Time = "
             << totalWT / processes.size();

        cout << "\n Average Turnaround Time = "
             << totalTAT / processes.size() << endl;
    }
// Displaying a Gantt chart
    void displayGanttChart()
    {
        cout << "\n\n Gantt Chart \n";
        int currentTime = 0;
        bool firstProcess = true;
        for (const auto& p : processes)
        {
            int startTime = p.getCompletionTime() - p.getBurstTime(); //Calculating the proccess's start time
            if (currentTime < startTime) //Checking id CPU is idle before the next proccess arrives
            {
                if (firstProcess)
                {
                    cout << currentTime;
                }
                cout << " -- Idle -- " << startTime;
            }
            else if (firstProcess)
            {
                cout << startTime;
            }
            // Display process execution interval
            cout << " -- " << p.getName() << " -- " << p.getCompletionTime();
            currentTime = p.getCompletionTime();
            firstProcess = false;
        }
        cout << endl;
    }
};

int main()
{
    FCFS scheduler;
// Reading and validating inputs
    if (!scheduler.inputProcesses())
    {
        return 1;
    }
// Displaying results as well as Gantt's chart
    scheduler.calculateFCFS();
    scheduler.displayResults();
    scheduler.displayGanttChart();

    return 0;
}

// Done wohooo!