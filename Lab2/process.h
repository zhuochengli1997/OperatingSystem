#ifndef PROCESS_H
#define PROCESS_H

struct Process {
    int arrivel_time;
    bool is_running_cpu;
    bool is_running_io;
    bool is_finished;
    int turnaround_time;
} process;

#endif