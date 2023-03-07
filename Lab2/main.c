#include <stdlib.h>
#include <stdio.h>

#include "process.h"

#define MAX_PROCESS 100
#define MAX_LINE 1024

int main() {
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    //a process between first number, and -1. after that, more or end;
    Process processes[MAX_PROCESS];
    int burst;
    int num_processes = 0;

    while (1) {
        Process p;
        p.arrivel_time = 0;
        p.num_io_bursts = 0;
        p.num_cpu_bursts = 0;

        if (scanf("%d", &p.arrivel_time) != 1) {
            break;
        }

        while (scanf("%d", &burst) == 1) {
            if (burst != -1) {
                p.cpu_burst[p.num_cpu_bursts] = burst;
                p.num_cpu_bursts++;
            } else break;
            scanf("%d", &burst);
            if (burst != -1) {
                p.io_burst[p.num_io_bursts] = burst;
                p.num_io_bursts++;
            } else break;
        }

        processes[num_processes] = p;
        num_processes++;

    }

    //print for testing
    for (int i = 0; i < num_processes; i++) {
        printf("Process %d:\n", i);
        printf("Arrival Time: %d\n", processes[i].arrivel_time);
        printf("CPU Bursts: ");
        for (int j = 0; j < processes[i].num_cpu_bursts; j++) {
            printf("%d ", processes[i].cpu_burst[j]);
        }
        printf("\nIO Bursts: ");
        for (int j = 0; j < processes[i].num_io_bursts; j++) {
            printf("%d ", processes[i].io_burst[j]);
        }
        printf("\n");
    }

}