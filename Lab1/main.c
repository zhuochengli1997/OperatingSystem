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

int main() {
    char input[MAX_INPUT_SIZE];

    while (true) {
        // char path[MAX_PATH];

        // if(getcwd(path,MAX_PATH) != NULL){
        //     printf("%s>",path);
        // }else{
        //     printf("shell> ");
        // }
        
        if (!read_input(input)) {
            break;
        }

        parse_input(input);
    }

    return EXIT_SUCCESS;
}
