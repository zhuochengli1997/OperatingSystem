#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <unistd.h>
#include "shell.h"
#include "scanner.h"

#define MAX_INPUT_SIZE 1024
#define MAX_PATH 255


/**
 * It reads a line of input from the user, and then parses it
 * 
 * @return The exit status of the program.
 */
int main() {
    char input[MAX_INPUT_SIZE];

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    while (true) {
        
        if (!read_input(input)) {
            break;
        }

        parse_input(input);
    }

    return EXIT_SUCCESS;
}
