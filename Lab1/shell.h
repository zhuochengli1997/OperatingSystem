#ifndef SHELL_H
#define SHELL_H

#include <stdbool.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

void execute_command(char *args[]);

bool read_input(char *input);

void parse_input(char *input);

#endif /* SHELL_H */
