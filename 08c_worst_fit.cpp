// Experiment 8(iii): Worst Fit memory allocation
// Each process goes into the LARGEST free block (so the leftover piece is big enough to reuse).
#include <iostream>
#include <vector>
using namespace std;

int main() {
    int m, n;
    cout << "Enter number of memory blocks: ";
    cin >> m;
    vector<int> block(m);
    cout << "Enter size of each block: ";
    for (int i = 0; i < m; i++) cin >> block[i];

    cout << "Enter number of processes: ";
    cin >> n;
    vector<int> proc(n);
    cout << "Enter size of each process: ";
    for (int i = 0; i < n; i++) cin >> proc[i];

    vector<int> alloc(n, -1);  // alloc[i] = block given to process i (-1 = not allocated)
    for (int i = 0; i < n; i++) {
        int worst = -1;
        for (int j = 0; j < m; j++)  // check every block, remember the largest one that fits
            if (block[j] >= proc[i] && (worst == -1 || block[j] > block[worst]))
                worst = j;

        if (worst != -1) {
            alloc[i] = worst;
            block[worst] -= proc[i];  // leftover space stays free in that block
        }
    }

    cout << "\nProcess\tSize\tBlock\n";
    for (int i = 0; i < n; i++) {
        cout << "P" << i + 1 << "\t" << proc[i] << "\t";
        if (alloc[i] != -1) cout << "B" << alloc[i] + 1 << "\n";
        else cout << "Not Allocated\n";
    }

    cout << "\nFree space left in blocks: ";
    for (int j = 0; j < m; j++) cout << "B" << j + 1 << "=" << block[j] << " ";
    cout << "\n";
    return 0;
}
