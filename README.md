# OS Lab: Concepts Guide

One C++ file per experiment. Each program reads its input from the keyboard and prints the result.
Every sample example in this guide gives exactly the same answer as the program, so you can type it in during the demo.

**How to compile and run** (example for experiment 1):

```bash
g++ 01_fcfs.cpp -o fcfs && ./fcfs
```

Each "Arrival time and burst time" prompt needs **two numbers on one line**, e.g. `0 5`.

| # | Experiment | File |
|---|---|---|
| 1 | First-Come-First-Served | `01_fcfs.cpp` |
| 2 | Shortest Job First | `02_sjf.cpp` |
| 3 | Round Robin | `03_round_robin.cpp` |
| 4 | Priority (preemptive) | `04_priority_preemptive.cpp` |
| 5 | Priority (non-preemptive) | `05_priority_non_preemptive.cpp` |
| 6 | Shortest Remaining Time First | `06_srtf.cpp` |
| 7(i) | Disk scheduling: FCFS | `07a_disk_fcfs.cpp` |
| 7(ii) | Disk scheduling: SSTF | `07b_disk_sstf.cpp` |
| 8(i) | First Fit | `08a_first_fit.cpp` |
| 8(ii) | Best Fit | `08b_best_fit.cpp` |
| 8(iii) | Worst Fit | `08c_worst_fit.cpp` |

**How each experiment section is laid out:** Rule → Sample example worked by hand → **Sample run** (exactly what you type and what the program prints) → **How the code works** → **Things to remember**.

---

## The C++ you need to read these programs

You only need a handful of C++ features. Learn these once and every program becomes easy to read.

| Code | What it means |
|---|---|
| `struct Process { int id, at, bt, ct, tat, wt; };` | A **struct** groups all the data about one process into one variable. `p[i].at` = arrival time of the i-th process. |
| `vector<Process> p(n);` | An array of `n` processes whose size is decided while the program runs. Use it exactly like an array: `p[i]`. |
| `cin >> p[i].at >> p[i].bt;` | Reads two numbers typed on one line with a space between them, e.g. `0 5`. |
| `vector<bool> done(n, false);` | `n` true/false flags, all starting as `false`. Used to mark "this process has finished". |
| `queue<int> ready;` | A real queue: `push(x)` adds at the back, `front()` reads the first, `pop()` removes the first. Used in Round Robin. |
| `abs(a - b)` | The distance between `a` and `b` (always positive). Used in disk scheduling. |
| `cout << fixed << setprecision(2);` | Print decimals with exactly 2 digits (11.25, 8.00). |
| `(double)total / n` | Convert to `double` before dividing, otherwise `45 / 4` gives `11` (integer division). |

**Sorting.** The FCFS and RR programs sort processes by arrival time:

```cpp
stable_sort(p.begin(), p.end(), [](const Process& a, const Process& b) { return a.at < b.at; });
```

The `[](...) { ... }` part is a **lambda**, a tiny unnamed function. It tells `sort` the rule: "`a` comes before `b` if `a` arrived earlier." `stable_sort` keeps ties in input order, so if P1 and P2 both arrive at 0, P1 stays first. If your examiner doesn't like lambdas, write a normal function instead. It does exactly the same thing:

```cpp
bool byArrival(Process a, Process b) { return a.at < b.at; }
...
stable_sort(p.begin(), p.end(), byArrival);
```

### The one pattern used in 7 of the 11 programs: "find the best one"

```cpp
int idx = -1;                                   // -1 = "found nothing yet"
for (int i = 0; i < n; i++)
    if (eligible(i) && (idx == -1 || better(i, idx)))
        idx = i;                                // i beats the best so far
// after the loop: idx is the winner, or -1 if nobody was eligible
```

Read it as: *go through everyone. If this one is allowed and is either the first allowed one or better than the best so far, it becomes the new best.* Only the two blanks change from program to program:

| Program | `eligible(i)`: who is allowed | `better(i, idx)`: who wins | Then the winner runs for |
|---|---|---|---|
| 2. SJF | arrived and not done | smaller **burst time** | its **full** burst |
| 5. Priority (non-preemptive) | arrived and not done | smaller **priority number** | its **full** burst |
| 6. SRTF | arrived and time left | smaller **remaining time** | **1** time unit, then choose again |
| 4. Priority (preemptive) | arrived and time left | smaller **priority number** | **1** time unit, then choose again |
| 7(ii). SSTF disk | not served yet | **closer** to the head | (move the head there) |
| 8(ii). Best Fit | block big enough | **smaller** block | (place the process) |
| 8(iii). Worst Fit | block big enough | **bigger** block | (place the process) |

That table is worth memorising: **"run full burst" = non-preemptive, "run 1 unit and re-choose" = preemptive.** That single difference is the whole gap between SJF and SRTF, and between experiments 5 and 4.

---

## Part A: CPU Scheduling (Experiments 1–6)

### The basics

- A **program** is a file on disk. When you run it, it becomes a **process**: a program that is currently running.
- A computer has many processes but (in our simple model) **one CPU**, and the CPU can run only one process at a time.
- Processes waiting for the CPU stand in a line called the **ready queue**.
- **CPU scheduling** is the OS deciding *which process in the ready queue gets the CPU next*. Different rules give different results, and that's what experiments 1–6 compare.

**Preemptive vs non-preemptive**
- **Non-preemptive:** once a process gets the CPU, it keeps it until it finishes. (FCFS, SJF, Priority non-preemptive)
- **Preemptive:** the OS can take the CPU away in the middle (when a better process arrives, or a time limit runs out). (Round Robin, SRTF, Priority preemptive)

Switching the CPU from one process to another is called a **context switch**. The OS must save the old process's state and load the new one's. This takes time, so too many switches is wasteful.

### The terms used in every example

| Term | Meaning | Formula |
|---|---|---|
| **AT**: Arrival Time | When the process enters the ready queue | given |
| **BT**: Burst Time | How much CPU time the process needs | given |
| **CT**: Completion Time | The moment the process finishes | read from the Gantt chart |
| **TAT**: Turnaround Time | Total time in the system, from arrival to finish | `TAT = CT − AT` |
| **WT**: Waiting Time | Time spent waiting in the queue, not running | `WT = TAT − BT` |

Lower average waiting time means a better schedule for that input.

### How to solve any CPU scheduling question (by hand or on the board)

1. Draw a timeline starting at time 0.
2. At each decision point, look **only at processes that have already arrived** and apply the algorithm's rule.
3. Draw the **Gantt chart**: boxes showing which process ran from when to when.
4. Read each process's **CT** from the chart (the time at the end of its last box).
5. Compute `TAT = CT − AT` and `WT = TAT − BT`, then take the averages.

The programs print the Gantt chart on one line like `(0) P1 (5) P2 (8)`, meaning P1 ran from 0 to 5, then P2 from 5 to 8. `idle` means no process had arrived yet, so the CPU sat empty.

---

### 1. First-Come-First-Served (FCFS)

**Rule:** whoever arrives first runs first, and runs to completion. Like a queue at a ticket counter.

**Type:** non-preemptive.

**Advantages:** the simplest algorithm, and fair in the sense of "first come, first served".

**Disadvantage, the convoy effect:** if a long process arrives first, every short process behind it has to wait for it to finish, like small cars stuck behind a slow truck. This makes the average waiting time high.

**In the program:** processes are sorted by arrival time and run one after another.

#### Sample example

| Process | AT | BT |
|---|---|---|
| P1 | 0 | 5 |
| P2 | 1 | 3 |
| P3 | 2 | 8 |
| P4 | 3 | 6 |

**Step by step**
- Order of arrival is P1, P2, P3, P4, so that is the order they run in.
- P1 arrives at 0 and runs 0 → 5.
- P2 arrived at 1 but must wait until P1 is done. It runs 5 → 8.
- P3 runs 8 → 16.
- P4 runs 16 → 22.

**Gantt chart**
```
| P1 | P2 |    P3    |   P4   |
0    5    8          16       22
```

**Calculation**

| Process | AT | BT | CT | TAT = CT − AT | WT = TAT − BT |
|---|---|---|---|---|---|
| P1 | 0 | 5 | 5 | 5 | 0 |
| P2 | 1 | 3 | 8 | 7 | 4 |
| P3 | 2 | 8 | 16 | 14 | 6 |
| P4 | 3 | 6 | 22 | 19 | 13 |

**Average TAT = 45 / 4 = 11.25, Average WT = 23 / 4 = 5.75**

**Convoy effect in action:** try BT = 24, 3, 3 (all arriving at 0). Average WT = (0 + 24 + 27) / 3 = **17**. If the two short jobs came first (3, 3, 24), it would be (0 + 3 + 6) / 3 = **3**. The same jobs in a different order give a very different result.

#### Sample run

```
Enter number of processes: 4
Arrival time and burst time of P1: 0 5
Arrival time and burst time of P2: 1 3
Arrival time and burst time of P3: 2 8
Arrival time and burst time of P4: 3 6

Gantt Chart: (0) P1 (5) P2 (8) P3 (16) P4 (22)

Process	AT	BT	CT	TAT	WT
P1	0	5	5	5	0
P2	1	3	8	7	4
P3	2	8	16	14	6
P4	3	6	22	19	13

Average Turnaround Time = 11.25
Average Waiting Time    = 5.75
```

#### How the code works (`01_fcfs.cpp`)

1. **Read input** into a `vector<Process>`. Each process gets `id = i + 1` so we can print P1, P2, …
2. **Sort by arrival time** (`stable_sort`). After this, the order of the vector *is* the FCFS order.
3. **Simulate with a clock** called `time`, starting at 0. For each process in order:
   - `if (time < x.at)` → the CPU is free but this process hasn't arrived yet, so it sits **idle** and the clock jumps to `x.at`.
   - `time += x.bt` → the process runs to completion.
   - `ct = time`, `tat = ct − at`, `wt = tat − bt`.
4. **Sort back by id** and print the table and the averages.

#### Things to remember
- FCFS = **sort by arrival + one loop**. It is the simplest program here; use it as the template for the others.
- The idle check `if (time < at) time = at;` is the line people forget. Without it, a process arriving at time 10 would "start" before it exists. Try input `2 3 / 10 2` and the program shows `(5) idle (10)`.
- For any non-preemptive algorithm, **WT = time it started − AT** is a quick way to check your table.
- The first process (if it arrives at 0) always has WT = 0.
- Averages must be computed with `double`, never `int`.

---

### 2. Shortest Job First (SJF), non-preemptive

**Rule:** whenever the CPU becomes free, pick the process with the **smallest burst time** among the processes that **have arrived**. It runs to completion.

**Type:** non-preemptive.

**Advantage:** it gives the **minimum average waiting time** of all non-preemptive algorithms, because short jobs get out of the way quickly and fewer processes are kept waiting.

**Disadvantages:**
- The OS does not really know burst times in advance. In practice they are estimated.
- **Starvation:** a long job may wait forever if short jobs keep arriving.

**In the program:** each time the CPU is free, it looks at all arrived, unfinished processes and runs the one with the smallest burst time.

#### Sample example

| Process | AT | BT |
|---|---|---|
| P1 | 0 | 7 |
| P2 | 2 | 4 |
| P3 | 4 | 1 |
| P4 | 5 | 4 |

**Step by step**
- **t = 0:** only P1 has arrived, so P1 runs. It is non-preemptive, so it runs fully: 0 → 7.
- **t = 7:** P2 (4), P3 (1) and P4 (4) are all waiting. The shortest is **P3**: 7 → 8.
- **t = 8:** P2 (4) and P4 (4) are tied. The tie goes to the one that came first, **P2**: 8 → 12.
- **t = 12:** only P4 is left: 12 → 16.

**Gantt chart**
```
|   P1   |P3|  P2  |  P4  |
0        7  8      12     16
```

**Calculation**

| Process | AT | BT | CT | TAT | WT |
|---|---|---|---|---|---|
| P1 | 0 | 7 | 7 | 7 | 0 |
| P2 | 2 | 4 | 12 | 10 | 6 |
| P3 | 4 | 1 | 8 | 4 | 3 |
| P4 | 5 | 4 | 16 | 11 | 7 |

**Average TAT = 32 / 4 = 8, Average WT = 16 / 4 = 4**

**Note:** P3 needed only 1 unit but still had to wait 3 units, because P1 could not be interrupted. SRTF (experiment 6) fixes exactly this.

#### Sample run

```
Enter number of processes: 4
Arrival time and burst time of P1: 0 7
Arrival time and burst time of P2: 2 4
Arrival time and burst time of P3: 4 1
Arrival time and burst time of P4: 5 4

Gantt Chart: (0) P1 (7) P3 (8) P2 (12) P4 (16)

Process	AT	BT	CT	TAT	WT
P1	0	7	7	7	0
P2	2	4	12	10	6
P3	4	1	8	4	3
P4	5	4	16	11	7

Average Turnaround Time = 8.00
Average Waiting Time    = 4.00
```

#### How the code works (`02_sjf.cpp`)

1. `done[i]` remembers which processes have finished. `completed` counts them.
2. `while (completed < n)`: each round is one scheduling decision.
3. **Choose** with the "find the best one" pattern: among processes that are **not done** and have **arrived** (`p[i].at <= time`), pick the smallest `bt`. Because the test is a strict `<`, on a tie the process typed **first** wins.
4. If nobody has arrived (`idx == -1`), the CPU is idle: jump the clock to the earliest unfinished arrival.
5. Otherwise **run it fully**: `time += bt`, fill in CT/TAT/WT, mark it done.

#### Things to remember
- The check `p[i].at <= time` is what makes this correct. Without it, the program would pick P3 (burst 1) at time 0, before P3 even exists.
- If **all processes arrive at 0**, SJF is just "sort by burst time and run in that order".
- SJF gives the **minimum average waiting time** among non-preemptive algorithms (it is provably optimal for that).
- Tie on burst time → the one that came first (FCFS) runs first. Enter processes in arrival order so this works naturally.
- Real OSs can't know the burst in advance; they *predict* it from past bursts (exponential averaging). Good to mention in the viva.

---

### 3. Round Robin (RR)

**Rule:** each process gets the CPU for a fixed small slice of time called the **time quantum**. If it hasn't finished when its quantum ends, it is stopped and sent to the **back** of the ready queue, and the next process gets a turn. Like friends taking turns on a game console, 10 minutes each.

**Type:** preemptive (the timer takes the CPU away).

**Advantages:** every process gets a turn regularly, so **no starvation**. Good response time, which is why it is used in time-sharing and interactive systems.

**The quantum size matters:**
- **Too large:** each process finishes within its turn, so RR behaves just like FCFS.
- **Too small:** the CPU spends a lot of time on context switches instead of real work.

**Queue rule used here:** if a new process arrives at the same moment a process's quantum ends, the **new process joins the queue first**, then the stopped process goes behind it.

**In the program:** a real queue. Take the front process, run it for one quantum (or less if it finishes), add new arrivals, then put it at the back if it still has work left.

#### Sample example (time quantum = 2)

| Process | AT | BT |
|---|---|---|
| P1 | 0 | 5 |
| P2 | 1 | 4 |
| P3 | 2 | 2 |
| P4 | 4 | 1 |

**Step by step** (the remaining time is in brackets)

| Time | Runs | What happens | Queue afterwards |
|---|---|---|---|
| 0 → 2 | P1 | P1 left: 3. P2 and P3 arrived meanwhile, then P1 goes to the back | P2, P3, P1 |
| 2 → 4 | P2 | P2 left: 2. P4 arrived at 4, then P2 goes to the back | P3, P1, P4, P2 |
| 4 → 6 | P3 | P3 needed only 2, so it **finishes at 6** | P1, P4, P2 |
| 6 → 8 | P1 | P1 left: 1 | P4, P2, P1 |
| 8 → 9 | P4 | P4 needed only 1, so it **finishes at 9** (gives up the CPU early) | P2, P1 |
| 9 → 11 | P2 | P2 **finishes at 11** | P1 |
| 11 → 12 | P1 | P1 **finishes at 12** | (empty) |

**Gantt chart**
```
| P1 | P2 | P3 | P1 |P4| P2 |P1|
0    2    4    6    8  9    11 12
```

**Calculation**

| Process | AT | BT | CT | TAT | WT |
|---|---|---|---|---|---|
| P1 | 0 | 5 | 12 | 12 | 7 |
| P2 | 1 | 4 | 11 | 10 | 6 |
| P3 | 2 | 2 | 6 | 4 | 2 |
| P4 | 4 | 1 | 9 | 5 | 4 |

**Average TAT = 31 / 4 = 7.75, Average WT = 19 / 4 = 4.75**

**Note:** P1 arrived first but finished last. RR cares about everyone getting regular turns, not about finishing the first process quickly.

#### Sample run

```
Enter number of processes: 4
Arrival time and burst time of P1: 0 5
Arrival time and burst time of P2: 1 4
Arrival time and burst time of P3: 2 2
Arrival time and burst time of P4: 4 1
Enter time quantum: 2

Gantt Chart: (0) P1 (2) P2 (4) P3 (6) P1 (8) P4 (9) P2 (11) P1 (12)

Process	AT	BT	CT	TAT	WT
P1	0	5	12	12	7
P2	1	4	11	10	6
P3	2	2	6	4	2
P4	4	1	9	5	4

Average Turnaround Time = 7.75
Average Waiting Time    = 4.75
```

#### How the code works (`03_round_robin.cpp`)

1. Each process has `rem` (remaining time), which starts equal to `bt`.
2. Processes are **sorted by arrival**. `next` points to the next process that hasn't entered the queue yet. Since they're sorted, "add everyone who has arrived" is just: `while (next < n && p[next].at <= time) ready.push(next++);`
3. Each round of the `while` loop:
   - Add new arrivals to the queue.
   - If the queue is **empty**, the CPU is idle: jump the clock to `p[next].at`.
   - Take the **front** process. It runs for `run = min(tq, rem)`: a full quantum, or less if it finishes early.
   - `time += run; rem -= run;`
   - **Add processes that arrived during this slice** to the queue.
   - Then, if `rem > 0`, push the current process to the **back**. Otherwise it's finished: fill in CT/TAT/WT.

#### Things to remember
- **Order matters:** new arrivals join the queue *before* the preempted process goes back. Swapping those two lines changes the Gantt chart. This is the most common RR mistake, on paper and in code.
- A process can finish in **less than** a quantum (P3 and P4 above). It gives up the CPU right away; the next process doesn't wait for the quantum to run out.
- If only one process is ready, it just gets quantum after quantum. The Gantt chart can show it twice in a row (e.g. `(2) P1 (4) P1 (5)`), which is correct.
- Quantum very large → RR becomes FCFS. Quantum very small → too many context switches.
- RR usually has a **higher average waiting time** than SJF, but much better **response time** (how soon each process first gets the CPU).

---

### 4. Priority Scheduling (preemptive)

**Rule:** every process has a **priority number**. In this lab, **smaller number = higher priority**. The CPU always runs the highest-priority process that has arrived. If a **higher-priority** process arrives while another is running, the running one is **interrupted** (preempted) and goes back to waiting.

**Type:** preemptive.

**Real-life analogy:** a hospital emergency room. A critical patient is attended to immediately, even if a minor case is already being treated.

**Disadvantage, starvation:** a low-priority process may never get the CPU if higher-priority processes keep arriving.
**Solution, aging:** slowly increase the priority of a process the longer it waits, so eventually it gets to run.

**In the program:** time moves forward 1 unit at a time, and at every unit the highest-priority arrived process is chosen. That is what allows a new arrival to interrupt.

#### Sample example

| Process | AT | BT | Priority |
|---|---|---|---|
| P1 | 0 | 5 | 3 |
| P2 | 1 | 3 | 1 (highest) |
| P3 | 2 | 2 | 2 |
| P4 | 3 | 4 | 4 (lowest) |

**Step by step**
- **t = 0:** only P1 is here, so P1 starts.
- **t = 1:** P2 (priority 1) arrives. It is higher than P1 (priority 3), so **P1 is preempted** with 4 units left. P2 runs.
- **t = 2:** P3 (priority 2) arrives. Not higher than P2 (1), so P2 keeps running.
- **t = 3:** P4 (priority 4) arrives. Not higher than P2, so P2 keeps running.
- **t = 4:** P2 finishes. Waiting: P1 (3), P3 (2), P4 (4). P3 has the highest priority: 4 → 6.
- **t = 6:** P1 (3) is higher than P4 (4). P1 runs its remaining 4 units: 6 → 10.
- **t = 10:** P4 runs: 10 → 14.

**Gantt chart**
```
|P1|  P2  | P3 |   P1   |   P4   |
0  1      4    6        10       14
```

**Calculation**

| Process | AT | BT | Pri | CT | TAT | WT |
|---|---|---|---|---|---|---|
| P1 | 0 | 5 | 3 | 10 | 10 | 5 |
| P2 | 1 | 3 | 1 | 4 | 3 | 0 |
| P3 | 2 | 2 | 2 | 6 | 4 | 2 |
| P4 | 3 | 4 | 4 | 14 | 11 | 7 |

**Average TAT = 28 / 4 = 7, Average WT = 14 / 4 = 3.5**

#### Sample run

Each line takes **three** numbers: arrival time, burst time, priority.

```
Enter number of processes: 4
Arrival time, burst time and priority of P1: 0 5 3
Arrival time, burst time and priority of P2: 1 3 1
Arrival time, burst time and priority of P3: 2 2 2
Arrival time, burst time and priority of P4: 3 4 4

Gantt Chart: (0) P1 (1) P2 (4) P3 (6) P1 (10) P4 (14)

Process	AT	BT	Pri	CT	TAT	WT
P1	0	5	3	10	10	5
P2	1	3	1	4	3	0
P3	2	2	2	6	4	2
P4	3	4	4	14	11	7

Average Turnaround Time = 7.00
Average Waiting Time    = 3.50
```

#### How the code works (`04_priority_preemptive.cpp`)

1. Each process has `rem` (remaining time) = `bt` at the start.
2. The loop moves the clock **one unit at a time**. Every unit it asks again: *"among processes that have arrived and still have time left, which has the smallest priority number?"*
3. That process runs for **one** unit: `rem--`, `time++`. If `rem` reaches 0, it's finished at `time`.
4. Because the choice is re-made every unit, a higher-priority process that just arrived wins the very next unit. **That is preemption**, and no special code is needed for it.
5. `last` remembers who ran in the previous unit. The program prints a new Gantt entry only when it changes, so the chart stays short.

#### Things to remember
- **Ask the examiner the convention.** This code uses *smaller number = higher priority*. If they want *bigger = higher*, change one character: `p[i].pr < p[idx].pr` → `p[i].pr > p[idx].pr`.
- Equal priority → the process typed first wins (strict `<`).
- A process can only be preempted when someone **arrives**, so on paper you only need to re-check at arrival and finish times.
- Starvation is the problem, **aging** is the fix.

---

### 5. Priority Scheduling (non-preemptive)

**Rule:** the choice is the same (the highest-priority process among those that have arrived), but once a process starts, it **runs to completion**. A higher-priority process that arrives meanwhile must wait for the current one to finish.

**Type:** non-preemptive.

Starvation and aging apply here too. Also note that **SJF is really priority scheduling where the priority is the burst time.**

**In the program:** same as SJF, except it compares the priority instead of the burst time.

#### Sample example (same input as experiment 4, to compare)

| Process | AT | BT | Priority |
|---|---|---|---|
| P1 | 0 | 5 | 3 |
| P2 | 1 | 3 | 1 (highest) |
| P3 | 2 | 2 | 2 |
| P4 | 3 | 4 | 4 (lowest) |

**Step by step**
- **t = 0:** only P1 is here, so it starts and **cannot be interrupted**: 0 → 5. (P2 arrives at 1 with a higher priority but has to wait.)
- **t = 5:** waiting: P2 (1), P3 (2), P4 (4). Highest priority is P2: 5 → 8.
- **t = 8:** P3 (2) beats P4 (4): 8 → 10.
- **t = 10:** P4: 10 → 14.

**Gantt chart**
```
|   P1   |  P2  | P3 |   P4   |
0        5      8    10       14
```

**Calculation**

| Process | AT | BT | Pri | CT | TAT | WT |
|---|---|---|---|---|---|---|
| P1 | 0 | 5 | 3 | 5 | 5 | 0 |
| P2 | 1 | 3 | 1 | 8 | 7 | 4 |
| P3 | 2 | 2 | 2 | 10 | 8 | 6 |
| P4 | 3 | 4 | 4 | 14 | 11 | 7 |

**Average TAT = 31 / 4 = 7.75, Average WT = 17 / 4 = 4.25**

**Comparing 4 and 5:** the most important process, P2, waited **0** units with preemption but **4** units without it. Preemptive priority serves important work faster, at the cost of extra context switches.

#### Sample run

```
Enter number of processes: 4
Arrival time, burst time and priority of P1: 0 5 3
Arrival time, burst time and priority of P2: 1 3 1
Arrival time, burst time and priority of P3: 2 2 2
Arrival time, burst time and priority of P4: 3 4 4

Gantt Chart: (0) P1 (5) P2 (8) P3 (10) P4 (14)

Process	AT	BT	Pri	CT	TAT	WT
P1	0	5	3	5	5	0
P2	1	3	1	8	7	4
P3	2	2	2	10	8	6
P4	3	4	4	14	11	7

Average Turnaround Time = 7.75
Average Waiting Time    = 4.25
```

#### How the code works (`05_priority_non_preemptive.cpp`)

It is the **SJF program with one change**: the comparison uses `p[i].pr` instead of `p[i].bt`. Everything else is the same: `done[]`, pick the best arrived process, jump over idle time, and run the winner for its **full** burst.

#### Things to remember
- If you can write SJF, you can write this. Change `bt` to `pr` in the comparison and read three numbers per process instead of two.
- **Experiment 4 vs 5 in one line:** 4 runs the winner for 1 unit and re-chooses; 5 runs the winner to the end.
- Same convention warning: smaller number = higher priority here.

---

### 6. Shortest Remaining Time First (SRTF)

**Rule:** the **preemptive version of SJF**. At every moment, the process with the **least remaining time** runs. When a new process arrives, compare its burst time with the **remaining** time of the running process. If the new one is shorter, it takes over.

**Type:** preemptive.

**Advantage:** gives the **lowest average waiting time** of all the algorithms in this lab.

**Disadvantages:** more context switches. Long jobs can starve. Like SJF, it needs to know burst times in advance.

**In the program:** each process tracks its remaining time. Time moves forward 1 unit at a time, and at every unit the arrived process with the least remaining time runs.

#### Sample example (same input as SJF, to compare)

| Process | AT | BT |
|---|---|---|
| P1 | 0 | 7 |
| P2 | 2 | 4 |
| P3 | 4 | 1 |
| P4 | 5 | 4 |

**Step by step** (remaining times in brackets)
- **t = 0:** only P1 (7), so P1 runs.
- **t = 2:** P2 arrives (4). P1 now has 7 − 2 = **5** left. 4 < 5, so **P1 is preempted**. P2 runs.
- **t = 4:** P3 arrives (1). P2 has **2** left. 1 < 2, so **P2 is preempted**. P3 runs.
- **t = 5:** P3 finishes. P4 arrives (4). Remaining: P1 (5), P2 (2), P4 (4). Least is P2, which runs 5 → 7 and finishes.
- **t = 7:** remaining: P1 (5), P4 (4). P4 runs 7 → 11.
- **t = 11:** P1 runs its last 5 units: 11 → 16.

**Gantt chart**
```
| P1 | P2 |P3| P2 |   P4   |    P1    |
0    2    4  5    7        11         16
```

**Calculation**

| Process | AT | BT | CT | TAT | WT |
|---|---|---|---|---|---|
| P1 | 0 | 7 | 16 | 16 | 9 |
| P2 | 2 | 4 | 7 | 5 | 1 |
| P3 | 4 | 1 | 5 | 1 | 0 |
| P4 | 5 | 4 | 11 | 6 | 2 |

**Average TAT = 28 / 4 = 7, Average WT = 12 / 4 = 3**

**Comparing SJF and SRTF:** on the same input, average WT dropped from **4 (SJF)** to **3 (SRTF)**, and P3 waited 0 instead of 3. But P1, the longest job, waited 9 instead of 0. The long job pays for the short ones.

#### Sample run

```
Enter number of processes: 4
Arrival time and burst time of P1: 0 7
Arrival time and burst time of P2: 2 4
Arrival time and burst time of P3: 4 1
Arrival time and burst time of P4: 5 4

Gantt Chart: (0) P1 (2) P2 (4) P3 (5) P2 (7) P4 (11) P1 (16)

Process	AT	BT	CT	TAT	WT
P1	0	7	16	16	9
P2	2	4	7	5	1
P3	4	1	5	1	0
P4	5	4	11	6	2

Average Turnaround Time = 7.00
Average Waiting Time    = 3.00
```

#### How the code works (`06_srtf.cpp`)

It is the **preemptive priority program with one change**: the comparison uses `p[i].rem` (remaining time) instead of `p[i].pr`. Every time unit, the arrived process with the least remaining time runs for one unit.

#### Things to remember
- Compare the newcomer with the **remaining** time of the running process, not its original burst. At t = 2, P1 had 5 left (not 7), and P2's 4 beat it.
- SRTF = **preemptive SJF**. It gives the lowest average waiting time of everything in this lab.
- For preemptive algorithms, **WT = TAT − BT** is the safe formula. "Start time − AT" does **not** work, because the process may run in several pieces.

### CPU scheduling: quick comparison

| Algorithm | Preemptive? | Picks by | Main problem |
|---|---|---|---|
| FCFS | No | arrival order | convoy effect |
| SJF | No | shortest burst | starvation, needs burst time in advance |
| SRTF | Yes | shortest remaining time | starvation, many switches |
| Round Robin | Yes | turn order + quantum | depends on quantum size |
| Priority | Either | priority number | starvation (fix: aging) |

---

## Part B: Disk Scheduling (Experiment 7)

### The basics

- A hard disk stores data on circular **tracks** (numbered 0, 1, 2, …), like rings on a record.
- A **read/write head** on an arm moves across the tracks to reach the data.
- Moving the head is the slowest part of reading data. The time to move it is the **seek time**, and it grows with the distance moved.
- Many programs request data at the same time, so there is a **request queue** of track numbers.
- **Disk scheduling** decides the order in which to serve these requests so the head moves **as little as possible**.

**What we measure:** **total head movement** = the sum of `|next track − current track|` over every move. Lower is better.

> Only FCFS and SSTF are included, as per the lab list. SCAN, C-SCAN, LOOK and C-LOOK were struck off.

Both examples below use the same input, so you can compare them:
**request queue = 98, 183, 37, 122, 14, 124, 65, 67** and **head starts at track 53**.

### 7(i). FCFS Disk Scheduling

**Rule:** serve requests in the exact order they arrived.

**Advantages:** simple and fair. Every request is served in turn, so there's no starvation.

**Disadvantage:** it ignores where the head currently is, so the head swings wildly back and forth across the disk, wasting time.

**In the program:** go through the queue in order and add up the distance of each move.

#### Sample example

| Step | From | To | Movement |
|---|---|---|---|
| 1 | 53 | 98 | 45 |
| 2 | 98 | 183 | 85 |
| 3 | 183 | 37 | 146 |
| 4 | 37 | 122 | 85 |
| 5 | 122 | 14 | 108 |
| 6 | 14 | 124 | 110 |
| 7 | 124 | 65 | 59 |
| 8 | 65 | 67 | 2 |
| | | **Total** | **640** |

**Seek sequence:** 53 → 98 → 183 → 37 → 122 → 14 → 124 → 65 → 67
**Total head movement = 640 tracks, average = 640 / 8 = 80 tracks per request**

**Observation:** look at steps 3–6. The head goes 183 → 37 → 122 → 14 → 124, crossing almost the whole disk four times. Notice also that 122 and 124 are right next to each other but are served far apart.

#### Sample run

All the request numbers go on **one line**, separated by spaces.

```
Enter number of requests: 8
Enter the request queue (track numbers): 98 183 37 122 14 124 65 67
Enter initial head position: 53

Seek sequence: 53 -> 98 -> 183 -> 37 -> 122 -> 14 -> 124 -> 65 -> 67

Total head movement = 640 tracks
Average seek length = 80 tracks
```

#### How the code works (`07a_disk_fcfs.cpp`)

1. `cur` = where the head is now (starts at `head`).
2. For each request **in the given order**: `total += abs(req[i] - cur)` adds the distance moved, then `cur = req[i]` moves the head there.
3. Print the total, and the average = total ÷ number of requests.

#### Things to remember
- The starting head position is **not** a request. It is only where the head begins.
- Distance is always `|to − from|`, never negative.
- Average seek length = total ÷ **number of requests** (8 here), not number of tracks.

### 7(ii). SSTF: Shortest Seek Time First

**Rule:** from the current head position, always go to the **nearest** pending request next. Repeat until all are served.

**Advantage:** much less total head movement than FCFS.

**Disadvantage, starvation:** a request far away from the head may keep getting skipped if new nearby requests keep arriving. It's the disk version of SJF.

**In the program:** at each step, check the distance to every unserved request and go to the closest one.

#### Sample example

At each step, compare the distance from the current head position to every request that is still pending:

| Step | Head at | Closest pending request | Movement |
|---|---|---|---|
| 1 | 53 | **65** (distance 12; the next closest is 67 at 14) | 12 |
| 2 | 65 | **67** (2) | 2 |
| 3 | 67 | **37** (30; 98 is 31) | 30 |
| 4 | 37 | **14** (23; 98 is 61) | 23 |
| 5 | 14 | **98** (84; only higher tracks are left) | 84 |
| 6 | 98 | **122** (24) | 24 |
| 7 | 122 | **124** (2) | 2 |
| 8 | 124 | **183** (59) | 59 |
| | | **Total** | **236** |

**Seek sequence:** 53 → 65 → 67 → 37 → 14 → 98 → 122 → 124 → 183
**Total head movement = 236 tracks, average = 236 / 8 = 29.5 tracks per request**

**Comparing 7(i) and 7(ii):** 640 vs 236. SSTF cuts head movement by almost two-thirds on the same requests. But request 183 had to wait until the very end, because it was far from where the head was working. That's how starvation starts.

#### Sample run

```
Enter number of requests: 8
Enter the request queue (track numbers): 98 183 37 122 14 124 65 67
Enter initial head position: 53

Seek sequence: 53 -> 65 -> 67 -> 37 -> 14 -> 98 -> 122 -> 124 -> 183

Total head movement = 236 tracks
Average seek length = 29.5 tracks
```

#### How the code works (`07b_disk_sstf.cpp`)

1. `served[i]` marks requests already handled.
2. Repeat `n` times (one move per request): use the "find the best one" pattern to find the **unserved request closest to `cur`**, i.e. the smallest `abs(req[i] - cur)`.
3. Add that distance to `total`, move the head there, mark it served.

#### Things to remember
- SSTF = the disk version of **SJF**: always do the nearest job next. Same strength (less total movement), same weakness (**starvation** of far-away requests).
- **Tie** (two requests at the same distance on both sides): this code picks the one that appears **first in the request queue**. If the examiner uses a different tie rule, say so.
- SSTF is good but **not always optimal**. It is greedy and only looks one step ahead.
- (Not in your list, but often asked in the viva: **SCAN** = the "elevator" algorithm, where the head sweeps to one end, then reverses. It was invented to fix SSTF's starvation.)

---

## Part C: Memory Allocation / Fit Strategies (Experiment 8)

### The basics

- Before a process can run, it must be loaded into **main memory (RAM)**.
- Free memory is available as **blocks (holes)** of different sizes, e.g. 100 KB, 500 KB, 200 KB…
- When a process needs memory, the OS must choose **which free block** to place it in. The rule it uses is the **fit strategy**.
- The process takes what it needs. The rest of that block stays free and can be used by later processes.
- If no single block is big enough, the process is **not allocated** and has to wait.

**External fragmentation:** the free memory gets broken into many small pieces. Even when the *total* free memory is enough, no *single* piece may be big enough for a process, so it can't be loaded. You'll see this happen in the examples below.

All three examples use the same input, so you can compare them:

| Block | B1 | B2 | B3 | B4 | B5 |
|---|---|---|---|---|---|
| Size (KB) | 100 | 500 | 200 | 300 | 600 |

| Process | P1 | P2 | P3 | P4 |
|---|---|---|---|---|
| Size (KB) | 212 | 417 | 112 | 426 |

Processes are placed one at a time, in order P1, P2, P3, P4.

### 8(i). First Fit

**Rule:** scan the blocks from the beginning and use the **first block that is big enough**.

**Advantage:** the **fastest** strategy, because it stops searching as soon as it finds a block.

**Disadvantage:** it doesn't think about which block is the best choice, so it can waste large blocks on small processes.

**In the program:** for each process, check blocks from B1 onwards and stop at the first one that fits.

#### Sample example

| Process | Search (current free size) | Placed in | Block left |
|---|---|---|---|
| P1 (212) | B1 (100) too small. **B2 (500) fits** | B2 | B2 = 288 |
| P2 (417) | B1 (100) ✗, B2 (288) ✗, B3 (200) ✗, B4 (300) ✗, **B5 (600) fits** | B5 | B5 = 183 |
| P3 (112) | B1 (100) ✗, **B2 (288) fits** | B2 | B2 = 176 |
| P4 (426) | B1 (100), B2 (176), B3 (200), B4 (300), B5 (183): **none fits** | **Not allocated** | |

**Final free space:** B1 = 100, B2 = 176, B3 = 200, B4 = 300, B5 = 183

**Observation, external fragmentation:** total free memory is 100 + 176 + 200 + 300 + 183 = **959 KB**, more than double what P4 needs (426 KB). Still, P4 can't be loaded, because no single block is big enough.

#### Sample run

Block sizes on one line, process sizes on one line.

```
Enter number of memory blocks: 5
Enter size of each block: 100 500 200 300 600
Enter number of processes: 4
Enter size of each process: 212 417 112 426

Process	Size	Block
P1	212	B2
P2	417	B5
P3	112	B2
P4	426	Not Allocated

Free space left in blocks: B1=100 B2=176 B3=200 B4=300 B5=183 
```

#### How the code works (`08a_first_fit.cpp`)

1. `block[j]` = free space currently left in block j. `alloc[i]` = which block process i got (`-1` = not allocated yet).
2. For each process, in order: loop over blocks from B1. At the **first** block with `block[j] >= proc[i]`, record it, subtract the process size from that block, and **`break`** (stop searching).
3. If the loop ends without a `break`, `alloc[i]` stays `-1` → print "Not Allocated".

#### Things to remember
- The **`break`** is what makes it First Fit. Remove it and the process would be placed in *every* block that fits, which is wrong.
- Blocks are 1-numbered in the output (`alloc[i] + 1`) because arrays start at 0.
- Processes are placed **in the order given**. An earlier process is never moved to make room for a later one.

### 8(ii). Best Fit

**Rule:** check **all** blocks and use the **smallest block that is big enough**. The idea is to leave as little wasted space as possible and keep big blocks free for big processes.

**Advantage:** big blocks are kept for big processes, so memory is often used better.

**Disadvantages:** slower, because every block must be checked. It also leaves behind many **tiny holes** that are too small to be useful.

**In the program:** for each process, check every block and remember the smallest one that fits.

#### Sample example

| Process | Blocks that fit (current free size) | Smallest one | Block left |
|---|---|---|---|
| P1 (212) | B2 (500), B4 (300), B5 (600) | **B4** | B4 = 88 |
| P2 (417) | B2 (500), B5 (600) | **B2** | B2 = 83 |
| P3 (112) | B3 (200), B5 (600) | **B3** | B3 = 88 |
| P4 (426) | B5 (600) | **B5** | B5 = 174 |

**Final free space:** B1 = 100, B2 = 83, B3 = 88, B4 = 88, B5 = 174

**Observation:** **all four processes fit**. Best Fit kept the 600 KB block untouched until the large P4 needed it. But look at the leftovers: 83, 88, 88. These are tiny holes that will be hard to use later.

#### Sample run

```
Enter number of memory blocks: 5
Enter size of each block: 100 500 200 300 600
Enter number of processes: 4
Enter size of each process: 212 417 112 426

Process	Size	Block
P1	212	B4
P2	417	B2
P3	112	B3
P4	426	B5

Free space left in blocks: B1=100 B2=83 B3=88 B4=88 B5=174 
```

#### How the code works (`08b_best_fit.cpp`)

For each process, use the "find the best one" pattern over **all** blocks: allowed = `block[j] >= proc[i]`, winner = the **smaller** block (`block[j] < block[best]`). No `break`, because you must see every block to know which is smallest. If `best != -1`, place the process there and subtract its size.

#### Things to remember
- Best = **smallest block that still fits**, not "the smallest block". A block that is too small is never considered.
- It compares the **current** free size of each block (after earlier processes took space), not the original size.

### 8(iii). Worst Fit

**Rule:** check **all** blocks and use the **largest block**. The idea is that the leftover piece will be big, so it's still useful for another process.

**Advantage:** avoids creating tiny unusable holes.

**Disadvantages:** slower (checks every block), and it uses up the big blocks early, so a large process arriving later may find nothing big enough.

**In the program:** for each process, check every block and remember the largest one that fits.

#### Sample example

| Process | Blocks that fit (current free size) | Largest one | Block left |
|---|---|---|---|
| P1 (212) | B2 (500), B4 (300), B5 (600) | **B5** | B5 = 388 |
| P2 (417) | B2 (500) (B5 is now only 388) | **B2** | B2 = 83 |
| P3 (112) | B3 (200), B4 (300), B5 (388) | **B5** | B5 = 276 |
| P4 (426) | B1 (100), B2 (83), B3 (200), B4 (300), B5 (276): **none fits** | **Not allocated** | |

**Final free space:** B1 = 100, B2 = 83, B3 = 200, B4 = 300, B5 = 276

**Observation:** Worst Fit cut into the 600 KB block first, for P1 and again for P3, so by the time P4 (426) came, the big block was gone.

#### Sample run

```
Enter number of memory blocks: 5
Enter size of each block: 100 500 200 300 600
Enter number of processes: 4
Enter size of each process: 212 417 112 426

Process	Size	Block
P1	212	B5
P2	417	B2
P3	112	B5
P4	426	Not Allocated

Free space left in blocks: B1=100 B2=83 B3=200 B4=300 B5=276 
```

#### How the code works (`08c_worst_fit.cpp`)

Identical to Best Fit except for one character: the winner is the **bigger** block (`block[j] > block[worst]`).

#### Things to remember
- The three fit programs share everything except the inner loop: **First** = stop at the first that fits; **Best** = smallest that fits; **Worst** = largest that fits.

### Fit strategies: comparison on this example

| Strategy | Picks | Speed | Result |
|---|---|---|---|
| First Fit | first block that fits | fastest | P4 not allocated |
| Best Fit | smallest block that fits | slower (checks all) | **all allocated** |
| Worst Fit | largest block | slower (checks all) | P4 not allocated |

No strategy is best for every input. On a different input, a different strategy could win.

### Watch out: two versions of this experiment exist

The programs above use **variable partitions**: after a process is placed, the leftover space in that block stays free, and a later process can use it. That's why P3 went into B2 under First Fit.

Some teachers use **fixed partitions** instead: **one block holds at most one process**, and the leftover inside it is wasted. That waste is called **internal fragmentation**. **Ask your examiner which version they expect.** To switch a program to fixed partitions:

```cpp
vector<bool> used(m, false);                          // add next to alloc
if (!used[j] && block[j] >= proc[i] ...)              // add !used[j] to the "fits" test
used[j] = true;                                       // instead of: block[j] -= proc[i];
```

Same input, fixed-partition results (checked by running the modified programs):

| Process | First Fit | Best Fit | Worst Fit |
|---|---|---|---|
| P1 (212) | B2 | B4 | B5 |
| P2 (417) | B5 | B2 | B2 |
| P3 (112) | **B3** | B3 | **B4** |
| P4 (426) | Not allocated | B5 | Not allocated |

Only P3's placement changes, because it can no longer share a block that's already in use.

**Internal vs external fragmentation (common viva question):**
- **Internal:** wasted space **inside** an allocated block (fixed partitions: 500 KB block, 417 KB process → 83 KB wasted inside).
- **External:** enough free memory **in total**, but split into pieces too small to use (variable partitions: P4 above). Fixed by **compaction**: moving processes together so the free pieces merge into one.

---

## Likely viva questions

- **Difference between preemptive and non-preemptive?** Whether the OS can take the CPU away from a running process.
- **Which CPU algorithm gives the minimum average waiting time?** SJF (non-preemptive) / SRTF (preemptive).
- **What is the convoy effect?** Short processes stuck waiting behind one long process in FCFS.
- **What is starvation, and how is it fixed?** A process waiting indefinitely. Fix: aging (raise priority over time).
- **What happens if the RR quantum is very large? Very small?** Large → becomes FCFS. Small → too many context switches.
- **What is a context switch?** Saving the state of the running process and loading the next one. This time is overhead.
- **Difference between turnaround time and waiting time?** Turnaround = whole time from arrival to finish. Waiting = only the time spent waiting in the queue (turnaround minus burst).
- **What is seek time?** Time for the disk head to move to the required track.
- **Why is SSTF better than FCFS for disks, and its drawback?** Less head movement. Drawback: far requests can starve.
- **What is external fragmentation?** Enough total free memory, but split into pieces too small to use.
- **Which fit is fastest?** First Fit. **Which wastes the least space per allocation?** Best Fit.
- **What is response time?** Time from arrival until the process **first** gets the CPU. RR is designed to keep it low.
- **Can the average waiting time be compared across algorithms?** Yes, on the same input. That's the point of the experiments. On this guide's inputs: SRTF (3.00) < SJF (4.00), and Priority preemptive (3.50) < non-preemptive (4.25).

---

## Exam-day checklist

1. **Write the formulas first** at the top of your answer sheet: `TAT = CT − AT`, `WT = TAT − BT`, `Avg = sum ÷ n`.
2. **Solve by hand before coding** if the question gives an input. Draw the Gantt chart. Then the program's output is your proof.
3. **Compile and run:**
   ```bash
   g++ 02_sjf.cpp -o sjf && ./sjf
   ```
4. **Type input carefully:** two numbers per line for AT BT, three for AT BT Priority. Request queue, block sizes and process sizes all go on **one line, separated by spaces**. Enter processes in arrival order.
5. **Ask two quick questions** if the paper doesn't say: *"Smaller priority number = higher priority?"* and, for fits, *"Can a block hold more than one process?"*
6. **If you have to write code from memory**, start from FCFS (simplest), then build the others from the "find the best one" pattern and its table near the top of this guide.
7. Typical mistakes to avoid: forgetting the idle case, integer division in averages, using "start − AT" for waiting time in **preemptive** algorithms, and the RR order of "new arrivals first, then the preempted process".
