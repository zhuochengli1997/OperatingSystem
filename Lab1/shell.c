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
 * Executes the "cd" command to change the working directory.
 * 
 * @param args The arguments of the cd command
 * @return 0 if successful, or a non-zero value indicating an error
 */
int execute_cd_command(char *args[]) {
    if (args[1] == NULL || args[1][0] == '\0') {
        printf("Error: cd requires folder to navigate to!\n");
        return 2;
    }
    if (chdir(args[1]) == -1) {
        printf("Error: cd directory not found!\n");
        return 2;
    }
    return 0;
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


/**
 * Splits the commands by "|", and splits each commands into separate arguments. Then it executes each
 * command in a separate process. This is for input containing "|".
*/
void execute_pipe_commands(char *input) {
    char *commands[MAX_ARGS];
    int command_count = split_pipeline(input, commands);

    int in_fd = STDIN_FILENO;
    pid_t child_pids[MAX_ARGS];

    for (int i = 0; i < command_count; i++) {
        char *args[MAX_ARGS];
        int arg_count = split_args(commands[i], args);
        args[arg_count] = NULL;

        if (strcmp(args[0], "exit") == 0) {
            exit(EXIT_SUCCESS);
        }

        // create pipe
        int fd[2];
        if (pipe(fd) == -1) {
            perror("Error pipe\n");
            exit(EXIT_FAILURE);
        }

        // create child process
        pid_t pid = fork();
        if (pid < 0) {
            perror("Error fork\n");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // child process
            if (in_fd != STDIN_FILENO) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (i < command_count - 1) {
                dup2(fd[1], STDOUT_FILENO);
            }
            close(fd[0]);
            close(fd[1]);
            execute_command(args);
            exit(EXIT_FAILURE);
        } else {
            // parent process
            child_pids[i] = pid;
            close(fd[1]);
            in_fd = fd[0];
        }
    }

    // wait for all child processes to terminate
    for (int i = 0; i < command_count; i++) {
        waitpid(child_pids[i], NULL, 0);
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
            else if (strcmp(args[0], "cd") == 0) {
                int cd_status = execute_cd_command(args);
                if (cd_status != 0) {
                    recent_exit_status = cd_status;
                    break;
                }
            }
            else {
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
    char *input_file = NULL;
    char *output_file = NULL;

    for (int i = 0; args[i] != NULL; i++) {
        if (strcmp(args[i], "<") == 0 && args[i+1] != NULL) {
            input_file = args[i+1];
        } else if (strcmp(args[i], ">") == 0 && args[i+1] != NULL) {
            output_file = args[i+1];
        }
    }
    if (input_file != NULL && output_file != NULL && strcmp(input_file, output_file) == 0) {
        printf("Error: input and output files cannot be equal!\n");
        exit(EXIT_FAILURE);
    }
    if (strcmp(args[0], "grep") == 0) {
        int arg_count = 0;
        while (args[arg_count] != NULL) {
            arg_count++;
        }
        if (arg_count >= 3 && strcmp(args[arg_count - 2], "|") != 0 && strcmp(args[arg_count - 2], "||") != 0) {
            args[arg_count] = args[arg_count - 1];
            args[arg_count - 1] = "-h";
            arg_count++;
        }
    }
    if (strcmp(args[0], "status") == 0) {
        printf("The most recent exit code is: %i\n", recent_exit_status);
        exit(EXIT_SUCCESS);
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