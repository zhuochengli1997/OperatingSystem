#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>

#define MAX_INPUT_SIZE 102400
#define MAX_ARGS 640

void execute_or_commands(char *input);

void execute_pipe_commands(char *input);

void execute_command(char *input[]);

bool read_input(char *input);

void parse_input(char *input);

int split_or(char *input, char *commands[]);

int split_commands(char *input, char *commands[]);

int split_pipeline(char *input, char *commands[]);

int split_args(char *command, char *args[]);

void check_exit(char* string);


#endif
