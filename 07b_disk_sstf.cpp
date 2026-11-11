// Experiment 7(ii): SSTF (Shortest Seek Time First) Disk Scheduling
// From the current head position, always serve the pending request that is closest.
#include <iostream>
#include <vector>
#include <cstdlib>
using namespace std;

int main() {
    int n, head;
    cout << "Enter number of requests: ";
    cin >> n;

    vector<int> req(n);
    cout << "Enter the request queue (track numbers): ";
    for (int i = 0; i < n; i++) cin >> req[i];
    cout << "Enter initial head position: ";
    cin >> head;

    vector<bool> served(n, false);
    int total = 0, cur = head;
    cout << "\nSeek sequence: " << cur;
    for (int k = 0; k < n; k++) {
        // Find the closest request that is not served yet
        int idx = -1;
        for (int i = 0; i < n; i++)
            if (!served[i] && (idx == -1 || abs(req[i] - cur) < abs(req[idx] - cur)))
                idx = i;

        total += abs(req[idx] - cur);
        cur = req[idx];
        served[idx] = true;
        cout << " -> " << cur;
    }

    cout << "\n\nTotal head movement = " << total << " tracks\n";
    cout << "Average seek length = " << (double)total / n << " tracks\n";
    return 0;
}
