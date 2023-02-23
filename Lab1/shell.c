#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"
#include "scanner.h"

void execute_command(char *args[]) {
    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        if (strcmp(args[0], "exit") == 0) {
            exit(EXIT_SUCCESS);
        }
        if (execvp(args[0], args) == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        int status;
        waitpid(pid, &status, 0);
    }
}

bool read_input(char *input) {
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
        return false;
    }
    input[strcspn(input, "\n")] = '\0';
    if (strcmp(input, "exit") == 0) {
        exit(EXIT_SUCCESS);
    }
    return true;
}

void execute_commands(char *input) {
    char *command;
    char *commands[MAX_ARGS];
    int command_count = 0;
    int i = 0;

    command = strtok(input, "&&||;");
    while (command != NULL && command_count < MAX_ARGS) {
        commands[command_count++] = command;
        command = strtok(NULL, "&&||;");
    }

    int status = 0;
    for (i = 0; i < command_count; i++) {
        char *args[MAX_ARGS];
        int arg_count = 0;
        char *arg = strtok(commands[i], " ");
        while (arg != NULL && arg_count < MAX_ARGS) {
            args[arg_count++] = arg;
            arg = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            if (strcmp(args[0], "exit") == 0) {
                exit(EXIT_SUCCESS);
            }
            if (execvp(args[0], args) == -1) {
                perror("execvp");
                exit(EXIT_FAILURE);
            }
        } else {
            waitpid(pid, &status, 0);
            if (status != 0) {
                break;
            }
        }
    }
}

void parse_input(char *input, char *args[]) {
    execute_commands(input);
}

