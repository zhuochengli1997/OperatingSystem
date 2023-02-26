#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "shell.h"
#include "scanner.h"

#define MAX_INPUT_SIZE 102400
#define MAX_PATH 2550


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
    char* username = getlogin();

    while (true) {
        if(getcwd(path,MAX_PATH) != NULL){
            printf("\033[1;36m%s\033[0m:\033[1;32m%s\033[0m>> ", username, path);
        }else{
            printf(">> ");
        }
        
        if (!read_input(input)) {   
            break;
        }

        parse_input(input);
    }

    return EXIT_SUCCESS;
}
