#include <stdlib.h>
#include <stdio.h>

#include "process.h"

#define MAX_PROCESS 100
#define MAX_LINE 1024
#define INF 1000000

int main() {
    //array of processes
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

    //FCFS scheduling algorithm
    int time = 0;
    int completed = 0;

    while (completed != num_processes) {
        //current process counter
        int cp = -1;
        int shortest = INF;

        //loop over processes to find shortest comeback time
        for (int i = 0; i < num_processes; i++) {
            Process proc = processes[i];
            if (proc.is_completed) continue;
            if (proc.current_cpu_burst == proc.num_cpu_bursts && !proc.is_completed) {
                //this process is completed now
                processes[i].turnaround_time = proc.comeback_at - proc.arrivel_time;
                completed++;
                processes[i].is_completed = 1;
                continue;
            }
            if (proc.arrivel_time > time) {
                continue; //this process has not arrived yet
            }
            if (proc.comeback_at < shortest) {
                shortest = proc.comeback_at;
                cp = i;
            }
        }

        if (time < processes[cp].comeback_at) {
            time = processes[cp].comeback_at;
        }

        Process proc = processes[cp]; //use to access more easily
        
        //check if wants to use the IO
        if (proc.current_io_burst != proc.num_io_bursts) {
            //wants to use IO, update comeback time accordingly
            processes[cp].comeback_at = time + proc.cpu_burst[proc.current_cpu_burst] + proc.io_burst[proc.current_io_burst];
            processes[cp].current_cpu_burst++;
            processes[cp].current_io_burst++;
        } else {
            //does not want to use IO, thus we can conclude it does not want to use the CPU anymore either. calc turnaround time and set to completed
            processes[cp].turnaround_time = time + proc.cpu_burst[proc.current_cpu_burst] - proc.arrivel_time;
            processes[cp].current_cpu_burst++;
            processes[cp].current_io_burst++;
            processes[cp].is_completed = 1;
            completed++;
        }
        
        //update the time according to when the CPU is available again
        time += processes[cp].cpu_burst[processes[cp].current_cpu_burst - 1];
    }

    //calculate average turnaround time
    float avgtime = 0;
    for (int j = 0; j < num_processes; j++) {
        avgtime += processes[j].turnaround_time;
    }

    avgtime = avgtime / num_processes;

    printf("%.f\n", avgtime);

    return 0;

}