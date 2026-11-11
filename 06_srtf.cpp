// Experiment 6: Shortest Remaining Time First (SRTF)
// Preemptive version of SJF: at every time unit, the arrived process with the least
// remaining time runs. A newly arrived shorter job takes the CPU immediately.
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
using namespace std;

struct Process {
    int id, at, bt;   // id, arrival time, burst time
    int rem;          // remaining burst time
    int ct, tat, wt;  // completion time, turnaround time, waiting time
};

int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> p(n);
    for (int i = 0; i < n; i++) {
        p[i].id = i + 1;
        cout << "Arrival time and burst time of P" << i + 1 << ": ";
        cin >> p[i].at >> p[i].bt;
        p[i].rem = p[i].bt;
    }

    int time = 0, completed = 0, last = -2;
    cout << "\nGantt Chart: ";
    while (completed < n) {
        // Every time unit, re-check which arrived process has the least remaining time
        int idx = -1;
        for (int i = 0; i < n; i++)
            if (p[i].rem > 0 && p[i].at <= time && (idx == -1 || p[i].rem < p[idx].rem))
                idx = i;

        if (idx != last) {  // a different process (or idle) starts here
            cout << "(" << time << ") " << (idx == -1 ? "idle" : "P" + to_string(p[idx].id)) << " ";
            last = idx;
        }

        time++;
        if (idx == -1) continue;  // CPU idle for this time unit

        p[idx].rem--;  // run the chosen process for one time unit
        if (p[idx].rem == 0) {
            p[idx].ct = time;
            p[idx].tat = p[idx].ct - p[idx].at;
            p[idx].wt = p[idx].tat - p[idx].bt;
            completed++;
        }
    }
    cout << "(" << time << ")\n\n";

    double totalTat = 0, totalWt = 0;
    cout << "Process\tAT\tBT\tCT\tTAT\tWT\n";
    for (auto& x : p) {
        cout << "P" << x.id << "\t" << x.at << "\t" << x.bt << "\t" << x.ct << "\t" << x.tat << "\t" << x.wt << "\n";
        totalTat += x.tat;
        totalWt += x.wt;
    }
    cout << fixed << setprecision(2);
    cout << "\nAverage Turnaround Time = " << totalTat / n << "\n";
    cout << "Average Waiting Time    = " << totalWt / n << "\n";
    return 0;
}
