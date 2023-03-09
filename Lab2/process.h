#ifndef PROCESS_H
#define PROCESS_H

typedef struct Process {
    int arrivel_time;
    int cpu_burst[100];
    int io_burst[100];
    int num_io_bursts;
    int num_cpu_bursts;
    int current_cpu_burst;
    int current_io_burst;
    int comeback_at;
    int is_completed;
    int turnaround_time;
} Process;

#endif