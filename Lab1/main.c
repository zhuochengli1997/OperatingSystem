#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "shell.h"
#include "scanner.h"

#define MAX_INPUT_SIZE 1024

int main() {
    char input[MAX_INPUT_SIZE];
    char *args[MAX_ARGS];

    while (true) {
        printf("shell> ");

        if (!read_input(input)) {
            break;
        }

        parse_input(input, args);
        execute_command(args);
    }

    return EXIT_SUCCESS;
}
