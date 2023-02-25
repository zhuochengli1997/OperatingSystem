#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"
#include "scanner.h"

int recent_exit_status = 0;

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

void execute_pipe_commands(char *input) {
    char *commands[MAX_ARGS];
    int command_count = split_piped(input, commands);

    int status = 0;
    for (int i = 0; i < command_count; i++) {
        char *args[MAX_ARGS];
        int arg_count = split_args(commands[i], args);
        args[arg_count] = NULL;

        if (strcmp(args[0], "exit") == 0) {
            exit(EXIT_SUCCESS);
        }
        
        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            execute_command(args);
        } else {
            waitpid(pid, &status, 0);
            if (WIFEXITED(status)) {
                recent_exit_status = WEXITSTATUS(status);
            }
            //status is 0, so previous command was executed and thus we do not need to execute the next command, thus break!
            if (status == 0) {
                break;
            }
        }
    }
}

void execute_commands(char *input) {
    char *commands[MAX_ARGS];
    int command_count = split_commands(input, commands);

    int status = 0;
    for (int i = 0; i < command_count; i++) {
        //To check if the command list contains the || character, such that we know we need to split the commands once again, and then execute
        if (strstr(commands[i], "||") != NULL) {
            execute_pipe_commands(commands[i]);
        }
        else {
            char *args[MAX_ARGS];
            int arg_count = split_args(commands[i], args);
            args[arg_count] = NULL;

            if (strcmp(args[0], "exit") == 0) {
                exit(EXIT_SUCCESS);
            }

            pid_t pid = fork();
            if (pid == -1) {
                perror("fork");
                exit(EXIT_FAILURE);
            }
            else if (pid == 0) {
                execute_command(args);
            } else {
                waitpid(pid, &status, 0);
                if (WIFEXITED(status)) {
                    recent_exit_status = WEXITSTATUS(status);
                }
                if (status != 0) {
                    break;
                }
            }
        }

    }
}

int split_piped(char *input, char *commands[]) {
    int command_count = 0;
    char *command = strtok(input, "||");
    while (command != NULL && command_count < MAX_ARGS) {
        commands[command_count++] = command;
        command = strtok(NULL, "||");
    }
    return command_count;
}

int split_commands(char *input, char *commands[]) {
    int command_count = 0;
    char *command = strtok(input, "&&;");
    while (command != NULL && command_count < MAX_ARGS) {
        commands[command_count++] = command;
        command = strtok(NULL, "&&;");
    }
    return command_count;
}

int split_args(char *command, char *args[]) {
    int arg_count = 0;
    char *arg = strtok(command, " ");
    while (arg != NULL && arg_count < MAX_ARGS) {
        args[arg_count++] = arg;
        arg = strtok(NULL, " ");
    }
    return arg_count;
}

void execute_command(char *args[]) {
    if (strcmp(args[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    }
    if (strcmp(args[0], "status") == 0) {
        printf("The most recent exit code is: %d\n", recent_exit_status);
    }
    else if (execvp(args[0], args) == -1) {
        //HERE, LOOK INTO POSSIBLE WAYS TO CHECK IF IT IS INVALID SYNTAX OR COMMAND NOT FOUND!
        fprintf(stderr, "Error, command not found!\n");
        //perror("execvp");
        exit(EXIT_FAILURE);
    }
}


void parse_input(char *input) {
    char* p = input;
    char* q = input;
    while (*q != '\0') {
        if (*q != '\"') {
            *p = *q;
            p++;
        }
        q++;
    }
    *p = '\0';
    execute_commands(input);
}