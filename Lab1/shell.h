#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>

#define MAX_INPUT_SIZE 10240
#define MAX_ARGS 640

void execute_command(char *args[]);

void execute_commands(char *input);

bool read_input(char *input);

void parse_input(char *input);

int split_piped(char *input, char *commands[]);

int split_commands(char *input, char *commands[]);

int split_args(char *command, char *args[]);

void check_exit(char* string);


#endif /* SHELL_H */
