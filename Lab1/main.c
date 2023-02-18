#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "scanner.h"
#include "shell.h"

int main(int argc, char *argv[]) {
    char *inputLine;
    List tokenList;

    //TODO: Signal back that the loop must stop when "exit" has been encountered (or EOF)
    while (true) {
        inputLine = readInputLine();
        tokenList = getTokenList(inputLine);

        bool parsedSuccessfully = parseInputLine(&tokenList);
        if (tokenList == NULL && parsedSuccessfully) {
            // Input was parsed successfully and can be accessed in "tokenList"

            // However, this is still a simple list of strings, it might be convenient
            // to build some intermediate structure representing the input line or a
            // command that you then construct in the parsing logic. It's up to you
            // to determine how to approach this!
        } else {
            printf("Error: invalid syntax!\n");
        }

        free(inputLine);
        freeTokenList(tokenList);
    }
    
    return 0;
}
