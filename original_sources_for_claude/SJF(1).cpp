#include <iostream>
#include <string>

using namespace std;

// 1] Struct process
struct process {
    string name;
    int arrive_time;
    int burst_time;

    int termination_time;
    int turnaround_time;
    int waiting_time;
};

// 2] Struct GanttEntry
struct GanttEntry {
    string process_name;
    int start;
    int end;
};

// 3] Class SJF (Non-Preemptive)
class SJF {
private:
    int no_of_processes;
    process* processes;
    GanttEntry* Gantt;
    bool* Is_Finished;
    int Gantt_count;

    bool isReady(int i, int currentTime) {
        return (Is_Finished[i] == false && processes[i].arrive_time <= currentTime);
    }

public:
    SJF(int size) {
        no_of_processes = size;
        processes = new process[no_of_processes];
        Gantt = new GanttEntry[no_of_processes];
        Is_Finished = new bool[no_of_processes];
        Gantt_count = 0;

        for (int i = 0; i < no_of_processes; i++) {
            Is_Finished[i] = false;
        }
    }

    void SetProcess(int index, string name, int arrive_time, int burst_time) {
        if (index >= 0 && index < no_of_processes) {
            processes[index].name = name;
            processes[index].arrive_time = arrive_time;
            processes[index].burst_time = burst_time;
            processes[index].termination_time = 0;
            processes[index].turnaround_time = 0;
            processes[index].waiting_time = 0;
            Is_Finished[index] = false;
        }
    }

    void calculate() {
        int currentTime = 0;
        int completed = 0;
        Gantt_count = 0;

        for (int i = 0; i < no_of_processes; i++) {
            Is_Finished[i] = false;
        }

        while (completed < no_of_processes) {
            int shortestIndex = -1;

            for (int i = 0; i < no_of_processes; i++) {
                if (isReady(i, currentTime)) {
                    if (shortestIndex == -1 || processes[i].burst_time < processes[shortestIndex].burst_time) {
                        shortestIndex = i;
                    }
                    else if (processes[i].burst_time == processes[shortestIndex].burst_time) {
                        if (processes[i].arrive_time < processes[shortestIndex].arrive_time) {
                            shortestIndex = i;
                        }
                    }
                }
            }

            if (shortestIndex == -1) {
                currentTime++;
            }
            else {
                int startTime = currentTime;
                currentTime += processes[shortestIndex].burst_time;

                processes[shortestIndex].termination_time = currentTime;
                processes[shortestIndex].turnaround_time =
                    processes[shortestIndex].termination_time - processes[shortestIndex].arrive_time;
                processes[shortestIndex].waiting_time =
                    processes[shortestIndex].turnaround_time - processes[shortestIndex].burst_time;

                Gantt[Gantt_count].process_name = processes[shortestIndex].name;
                Gantt[Gantt_count].start = startTime;
                Gantt[Gantt_count].end = currentTime;
                Gantt_count++;

                Is_Finished[shortestIndex] = true;
                completed++;
            }
        }
    }

    double get_total_waiting_time() {
        double total = 0;
        for (int i = 0; i < no_of_processes; i++) {
            total += processes[i].waiting_time;
        }
        return total;
    }

    double get_Avg_waiting_time() {
        if (no_of_processes == 0) return 0.0;
        return get_total_waiting_time() / no_of_processes;
    }

    double get_total_turnaround() {
        double total = 0;
        for (int i = 0; i < no_of_processes; i++) {
            total += processes[i].turnaround_time;
        }
        return total;
    }

    double get_Avg_turnaround() {
        if (no_of_processes == 0) return 0.0;
        return get_total_turnaround() / no_of_processes;
    }

    process* getProcesses() { return processes; }
    GanttEntry* getGanttChart() { return Gantt; }
    int getGanttCount() { return Gantt_count; }

    ~SJF() {
        delete[] processes;
        delete[] Gantt;
        delete[] Is_Finished;
    }
};

//  main() for Inputs and Outputs ---
int main() {
    int n = 4;
    SJF sjf(n);

    // 1. Inputs
    sjf.SetProcess(0, "P1", 0, 6);
    sjf.SetProcess(1, "P2", 0, 8);
    sjf.SetProcess(2, "P3", 0, 7);
    sjf.SetProcess(3, "P4", 0, 3);

    // 2. Run Calculation
    sjf.calculate();

    // 3. Outputs: Gantt Chart Data (Start & End times)
    cout << "Gantt Order:\n";
    GanttEntry* gantt = sjf.getGanttChart();
    for (int i = 0; i < sjf.getGanttCount(); i++) {
        cout << gantt[i].process_name << ": " << gantt[i].start << " to " << gantt[i].end << endl;
    }

    // 4. Outputs: Each Process's Calculated Times
    cout << "\nProcess Results:\n";
    process* p = sjf.getProcesses();
    for (int i = 0; i < n; i++) {
        cout << p[i].name
            << " | CT: " << p[i].termination_time
            << " | TAT: " << p[i].turnaround_time
            << " | WT: " << p[i].waiting_time << endl;
    }

    // 5. Outputs: Totals & Averages
    cout << "\nTotal Waiting Time: " << sjf.get_total_waiting_time() << endl;
    cout << "Avg Waiting Time: " << sjf.get_Avg_waiting_time() << endl;
    cout << "Total Turnaround Time: " << sjf.get_total_turnaround() << endl;
    cout << "Avg Turnaround Time: " << sjf.get_Avg_turnaround() << endl;

    return 0;
}