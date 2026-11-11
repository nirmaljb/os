// Experiment 7(i): FCFS Disk Scheduling
// The disk head serves track requests exactly in the order they arrived.
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

    int total = 0, cur = head;
    cout << "\nSeek sequence: " << cur;
    for (int i = 0; i < n; i++) {
        total += abs(req[i] - cur);  // distance the head moves for this request
        cur = req[i];
        cout << " -> " << cur;
    }

    cout << "\n\nTotal head movement = " << total << " tracks\n";
    cout << "Average seek length = " << (double)total / n << " tracks\n";
    return 0;
}
