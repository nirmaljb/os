// Experiment 5: Priority Scheduling (non-preemptive)
// Among the arrived processes, the one with the highest priority runs next and keeps
// the CPU until it finishes.
// Convention: smaller number = higher priority.
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <climits>
using namespace std;

struct Process {
    int id, at, bt, pr;  // id, arrival time, burst time, priority
    int ct, tat, wt;     // completion time, turnaround time, waiting time
};

int main() {
    int n;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> p(n);
    for (int i = 0; i < n; i++) {
        p[i].id = i + 1;
        cout << "Arrival time, burst time and priority of P" << i + 1 << ": ";
        cin >> p[i].at >> p[i].bt >> p[i].pr;
    }

    vector<bool> done(n, false);
    int time = 0, completed = 0;
    cout << "\nGantt Chart: ";
    while (completed < n) {
        // Pick the highest-priority process among those that have arrived
        int idx = -1;
        for (int i = 0; i < n; i++)
            if (!done[i] && p[i].at <= time && (idx == -1 || p[i].pr < p[idx].pr))
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
    cout << "Process\tAT\tBT\tPri\tCT\tTAT\tWT\n";
    for (auto& x : p) {
        cout << "P" << x.id << "\t" << x.at << "\t" << x.bt << "\t" << x.pr << "\t" << x.ct << "\t" << x.tat << "\t" << x.wt << "\n";
        totalTat += x.tat;
        totalWt += x.wt;
    }
    cout << fixed << setprecision(2);
    cout << "\nAverage Turnaround Time = " << totalTat / n << "\n";
    cout << "Average Waiting Time    = " << totalWt / n << "\n";
    return 0;
}
