#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "shell.h"
#include "scanner.h"

//variable to hold the most recent exit status
int recent_exit_status = 0;

/**
 * It reads a line of input from the user, and if the user typed "exit", it exits the program
 * 
 * @param input The input string.
 * 
 * @return a boolean value indicating whether reading the input was succesfull or not
 */
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

/**
 * It splits the input into commands, then splits each command into arguments, then executes each
 * command in a separate process. This is for the input containing "||".
 * 
 * @param input The input string that the user entered.
 */
void execute_or_commands(char *input) {
    char *commands[MAX_ARGS];
    int command_count = split_or(input, commands);
    
    int before_status = 0;
    int status = 0;
    for (int i = 0; i < command_count; i++) {
        char *args[MAX_ARGS];
        int arg_count = split_args(commands[i], args);
        args[arg_count] = NULL;

        before_status = recent_exit_status;

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
            /*the previous exit status of the argument before the "||" was 1, and after the "||" we have status. then, we need to display the exit 
            status of the previous command (and not the command before the "||")*/
            if (recent_exit_status == 1 && i < command_count - 1 && strstr(commands[i + 1], "status") != NULL) {
                recent_exit_status = before_status;
            }
            //status is 0, so previous command was executed and we do not need to execute the next command, thus break!
            if (status == 0) {
                break;
            }
        }
    }
}

/* Function to get the input file for a command */
char *get_input_file(char *command) {
    char *input_file = NULL;
    char *token;
    token = strtok(command, "<");

    /* If there is an input file, extract it */
    if (token != NULL && strtok(NULL, "<") != NULL) {
        input_file = strtok(NULL, "<");
        /* Remove any leading or trailing spaces */
        input_file[strspn(input_file, " ")] = 0;
        int len = strlen(input_file);
        input_file[len - strspn(input_file, " \r\n")] = 0;
    }
    return input_file;
}

/* Function to get the output file for a command */
char *get_output_file(char *command) {
    char *output_file = NULL;
    char *token;
    token = strtok(command, ">");

    /* If there is an output file, extract it */
    if (token != NULL && strtok(NULL, ">") != NULL) {
        output_file = strtok(NULL, ">");
        /* Remove any leading or trailing spaces */
        output_file[strspn(output_file, " ")] = 0;
        int len = strlen(output_file);
        output_file[len - strspn(output_file, " \r\n")] = 0;
    }
    return output_file;
}


/**
 * Splits the commands by "|", and splits each commands into separate arguments. Then it executes each
 * command in a separate process. This is for input containing "|".
*/
void execute_pipe_commands(char *input) {
    char *commands[MAX_ARGS];
    int command_count = split_pipeline(input, commands);

    for (int i = 0; i < command_count; i+=2) {
        char *args[MAX_ARGS];
        char *args2[MAX_ARGS];
        int arg_count = split_args(commands[i], args);
        int arg2_count = split_args(commands[i+1], args2);
        args[arg_count] = NULL;
        args2[arg2_count] = NULL;

        if (strcmp(args[0], "exit") == 0) {
            exit(EXIT_SUCCESS);
        }

        //create pipe
        int fd[2];
        if (pipe(fd) == -1) {
            perror("Error pipe\n");
            exit(EXIT_FAILURE);
        }

        //create processes
        int pid1 = fork();
        if (pid1 < 0) {
            perror("Error pid1\n");
            exit(EXIT_FAILURE);
        }

        if (pid1 == 0) {
            //?
            dup2(fd[1], STDOUT_FILENO);
            close(fd[0]);
            close(fd[1]);
            //child process 1 for program "a"
            execute_command(args);
        }

        int pid2 = fork();
        if (pid2 < 0) {
            perror("Error pid2\n");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0) {
            //child process 2 (right hand side)
            dup2(fd[0], STDIN_FILENO);
            close(fd[0]);
            close(fd[1]);
            execute_command(args2);
        }

        close(fd[0]);
        close(fd[1]);

        waitpid(pid1, NULL, 0);
        waitpid(pid2, NULL, 0);
    }

}

/**
 * It splits the commands by the ";" and "&&", and then creates an argument array for each command while looping over the commands.
 * Then it executes each command in a seperate process
 * 
 * @param input The input string that the user entered
 */
void execute_commands(char *input) {
    char *commands[MAX_ARGS];
    int command_count = split_commands(input, commands);

    int status = 0;
    for (int i = 0; i < command_count; i++) {
        //To check if the command list contains the || character, such that we know we need to call execute_or_commands
        if (strstr(commands[i], "||") != NULL) {
            execute_or_commands(commands[i]);
        }
        else if (strstr(commands[i], "|") != NULL){
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

/**
 * It takes a composed command and splits it into an array of commands, seperated by "||". 
 * 
 * @param input The input command to be split.
 * @param commands This is the array of commands that will be returned.
 * 
 * @return The number of commands in the input.
 */
int split_or(char *input, char *commands[]) {
    int command_count = 0;
    char *command = strtok(input, "||");
    while (command != NULL && command_count < MAX_ARGS) {
        commands[command_count++] = command;
        command = strtok(NULL, "||");
    }
    return command_count;
}

/**
 * It takes a composed command and splits it into an array of commands, seperated by ";" or "&&".
 * 
 * @param input The input command to be split.
 * @param commands This is the array of commands that will be returned.
 * 
 * @return The number of commands in the input string.
 */
int split_commands(char *input, char *commands[]) {
    int command_count = 0;
    char *command = strtok(input, "&&;");
    while (command != NULL && command_count < MAX_ARGS) {
        commands[command_count++] = command;
        command = strtok(NULL, "&&;");
    }
    return command_count;
}

/**
 * This function should take a pipelined command and split it into an array of commands, seperated by "|".
 * 
 * @param input The input command to be split.
 * @param commands This is the array of commands that will be returned.
 * 
 * @return The number of commands in the input string
*/
int split_pipeline(char *input, char *commands[]) {
    int command_count = 0;
    char *command = strtok(input, "|");
    while (command != NULL && command_count < MAX_ARGS) {
        commands[command_count++] = command;
        command = strtok(NULL, "|");
    }
    return command_count;
}

/**
 * It takes a command and splits it into an array of arguments of the command, seperated by a space
 * 
 * @param command The command to be split into arguments.
 * @param args This is the array of arguments that will hold the result after splitting
 * 
 * @return The number of arguments in args
 */
int split_args(char *command, char *args[]) {
    int arg_count = 0;
    char *arg = strtok(command, " ");
    while (arg != NULL && arg_count < MAX_ARGS) {
        args[arg_count++] = arg;
        arg = strtok(NULL, " ");
    }
    return arg_count;
}

/**
 * This function executes the command that the user has inputted.
 * 
 * @param args This is the array of arguments that is a single command.
 */
void execute_command(char *args[]) {
    if (strcmp(args[0], "exit") == 0) {
        exit(EXIT_SUCCESS);
    }
    if (strcmp(args[0], "status") == 0) {
        printf("The most recent exit code is: %i\n", recent_exit_status);
        exit(EXIT_SUCCESS);
    }
    if (strcmp(args[0],"cd") == 0){
        if (args[1] == NULL || args[1][0] == '\0') {
            printf("Error: cd requires folder to navigate to!\n");
            exit(2);
        }
        if (chdir(args[1]) == -1) {
            printf("Error: cd directory not found!\n");
            exit(2);
        }
    }
    else if (execvp(args[0], args) == -1) {
        fprintf(stdout, "Error: command not found!\n");
        exit(127);
    }
}


/**
 * It removes all the double quotes from the input string
 * 
 * @param input The input string
 */
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