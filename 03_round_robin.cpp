// Experiment 3: Round Robin (RR) CPU Scheduling
// Each process gets the CPU for at most one time quantum, then goes to the back of the
// ready queue if it still has work left.
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <algorithm>
using namespace std;

struct Process {
    int id, at, bt;   // id, arrival time, burst time
    int rem;          // remaining burst time
    int ct, tat, wt;  // completion time, turnaround time, waiting time
};

int main() {
    int n, tq;
    cout << "Enter number of processes: ";
    cin >> n;

    vector<Process> p(n);
    for (int i = 0; i < n; i++) {
        p[i].id = i + 1;
        cout << "Arrival time and burst time of P" << i + 1 << ": ";
        cin >> p[i].at >> p[i].bt;
        p[i].rem = p[i].bt;
    }
    cout << "Enter time quantum: ";
    cin >> tq;

    stable_sort(p.begin(), p.end(), [](const Process& a, const Process& b) { return a.at < b.at; });

    queue<int> ready;  // ready queue holds indexes into p
    int time = 0, completed = 0, next = 0;  // next = next process (by arrival) not yet in the queue
    cout << "\nGantt Chart: ";
    while (completed < n) {
        while (next < n && p[next].at <= time) ready.push(next++);  // add new arrivals

        if (ready.empty()) {  // CPU idle until the next process arrives
            cout << "(" << time << ") idle ";
            time = p[next].at;
            continue;
        }

        int i = ready.front();
        ready.pop();
        int run = min(tq, p[i].rem);  // run for one quantum, or less if it finishes early
        cout << "(" << time << ") P" << p[i].id << " ";
        time += run;
        p[i].rem -= run;

        // Processes that arrived during this slice enter the queue before the current one goes back
        while (next < n && p[next].at <= time) ready.push(next++);

        if (p[i].rem > 0) {
            ready.push(i);  // not finished: back of the queue
        } else {
            p[i].ct = time;
            p[i].tat = p[i].ct - p[i].at;
            p[i].wt = p[i].tat - p[i].bt;
            completed++;
        }
    }
    cout << "(" << time << ")\n\n";

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
