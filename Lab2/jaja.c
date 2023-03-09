#include <stdlib.h>
#include <stdio.h>

#include "process.h"

#define MAX_PROCESS 100
#define MAX_LINE 1024
#define INFINITY 1000000

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
        p.current_cpu_burst = 0;
        p.current_io_burst = 0;
        p.turnaround_time = 0;
        p.is_completed = 0;

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
        //set comeback time to arrival time for the scheduling algorithm below
        processes[num_processes].comeback_at = processes[num_processes].arrivel_time;
        num_processes++;

    }

    //print for testing
    /*for (int i = 0; i < num_processes; i++) {
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
    }*/

    //FCFS scheduling algorithm
    int time = 0;
    int completed = 0;


    while (completed != num_processes) {
        //current process counter
        int cp = -1;
        int shortest = INFINITY;

        //loop over processes to find shortest comeback time
        for (int i = 0; i < num_processes; i++) {
            Process proc = processes[i];
            if (proc.is_completed) continue;
            if (proc.current_cpu_burst == proc.num_cpu_bursts && !proc.is_completed) {
                processes[i].turnaround_time = time - proc.arrivel_time;
                completed++;
                processes[i].is_completed = 1;
                continue; //this process is completed
            }
            if (proc.arrivel_time > time) {
                continue; //this process has not arrived yet
            }
            if (proc.comeback_at < shortest) {
                shortest = proc.comeback_at;
                cp = i;
            }
        }
        //up untill here, we have the shortest comeback time
        processes[cp].comeback_at = time + processes[cp].cpu_burst[processes[cp].current_cpu_burst] + processes[cp].io_burst[processes[cp].current_io_burst];
        //printf("comeback time!: %d\n", processes[cp].comeback_at);
        //comeback time = time + cpuburst time + ioburst time ^^^

        //we executed an CPU burst, thus update
        processes[cp].current_cpu_burst++;

        //now we check if there is a IO burst left, and if so, check if it was last one. If it was last one, add this io time to turnaround
        //if not last one, we continue because there might be more CPU and IO left

        if (processes[cp].num_io_bursts != processes[cp].current_io_burst) {
            //this means there is a IO burst left. check if it is the last one!
            if (processes[cp].num_cpu_bursts == processes[cp].current_cpu_burst) {
                //this IO is the final burst of the process
                processes[cp].is_completed = 1;
                completed++;
                //process is done after coming back from IO time, thus use comeback time
                processes[cp].turnaround_time = processes[cp].comeback_at;
            } else {
                //this IO is not the final process, there is another cpu time so we do nothing
                processes[cp].current_io_burst++;
            }
        } else if (processes[cp].num_cpu_bursts == processes[cp].current_cpu_burst) {
            processes[cp].is_completed = 1;
            completed++;
            processes[cp].turnaround_time = time + processes[cp].cpu_burst[processes[cp].current_cpu_burst - 1];
            printf("Now we finish here! %d\n", cp);
        }

        time += processes[cp].cpu_burst[processes[cp].current_cpu_burst - 1];

        printf("Normal time updated to: %d\n", time);
    }

    int avgtime = 0;
    for (int j = 0; j < num_processes; j++) {
        printf("process %d took %d\n", j, processes[j].turnaround_time);
        avgtime += processes[j].turnaround_time;
    }

    avgtime = avgtime / num_processes;

    printf("%d\n", avgtime);

    return 0;

}