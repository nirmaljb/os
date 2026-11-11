// Experiment 2: Shortest Job First (SJF) CPU Scheduling (non-preemptive)
// Among the processes that have arrived, the one with the smallest burst time runs next,
// and it keeps the CPU until it finishes.
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <climits>
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

    vector<bool> done(n, false);
    int time = 0, completed = 0;
    cout << "\nGantt Chart: ";
    while (completed < n) {
        // Pick the shortest job among those that have arrived
        int idx = -1;
        for (int i = 0; i < n; i++)
            if (!done[i] && p[i].at <= time && (idx == -1 || p[i].bt < p[idx].bt))
                idx = i;

        if (idx == -1) {  // nothing has arrived yet: CPU is idle till the next arrival
            int next = INT_MAX;
            for (int i = 0; i < n; i++)
                if (!done[i]) next = min(next, p[i].at);
            cout << "(" << time << ") idle ";
            time = next;
            continue;
        }

        cout << "(" << time << ") P" << p[idx].id << " ";
        time += p[idx].bt;  // runs to completion
        p[idx].ct = time;
        p[idx].tat = p[idx].ct - p[idx].at;
        p[idx].wt = p[idx].tat - p[idx].bt;
        done[idx] = true;
        completed++;
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
