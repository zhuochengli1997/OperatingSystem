#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "shell.h"
#include "scanner.h"

#define MAX_INPUT_SIZE 102400
#define MAX_PATH 2550

#ifdef EXT_PROMPT
#include <unistd.h>
#include <pwd.h>
#define RESET_COLOR "\033[0m"
#define BOLD_GREEN "\033[1m\033[32m"
#define BOLD_CYAN "\033[1m\033[36m"
#endif

/**
 * It reads a line of input from the user, and then parses it
 * 
 * @return The exit status of the program.
 */
int main() {
    char input[MAX_INPUT_SIZE];
    char path[MAX_PATH];
    
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    while (true) {
#ifdef EXT_PROMPT
        char *username = getpwuid(getuid())->pw_name;
        if (getcwd(path, MAX_PATH) != NULL) {
            printf("%s%s@%s%s:%s%s%s> ", BOLD_GREEN, username, BOLD_CYAN, path, RESET_COLOR, BOLD_GREEN, "$");
        } else {
            printf("%s%s@%s%s:%s%s%s> ", BOLD_GREEN, username, BOLD_CYAN, "unknown", RESET_COLOR, BOLD_GREEN, "$");
        }
#else
        if (getcwd(path, MAX_PATH) != NULL) {
            printf("%s> ", path);
        } else {
            printf("> ");
        }
#endif
        
        if (!read_input(input)) {   
            break;
        }

        parse_input(input);
    }

    return EXIT_SUCCESS;
}
