// Experiment 1: First-Come-First-Served (FCFS) CPU Scheduling
// The process that arrives first gets the CPU first and keeps it until it finishes.
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
using namespace std;

struct Process {
    int id, at, bt;   // id, arrival time, burst time
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
    }

    // Serve processes in the order they arrive
    stable_sort(p.begin(), p.end(), [](const Process& a, const Process& b) { return a.at < b.at; });

    int time = 0;
    cout << "\nGantt Chart: ";
    for (auto& x : p) {
        if (time < x.at) {  // CPU sits idle until the next process arrives
            cout << "(" << time << ") idle ";
            time = x.at;
        }
        cout << "(" << time << ") P" << x.id << " ";
        time += x.bt;  // runs to completion

        x.ct = time;
        x.tat = x.ct - x.at;
        x.wt = x.tat - x.bt;
    }
    cout << "(" << time << ")\n\n";

    // Print results in process order
    sort(p.begin(), p.end(), [](const Process& a, const Process& b) { return a.id < b.id; });
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
