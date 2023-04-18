#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <unistd.h>
#include <signal.h>
#include "shell.h"

#define MAX_INPUT_SIZE 102400
#define MAX_PATH 2550

// signal handler for SIGINT
void ctrl_c_handler(int signum)
{
    if (getNumBgProcs() != 0)
    {
        printf("Error: there are still background processes running!\n");
    }
    else
    {
        exit(EXIT_SUCCESS);
    }
}

/**
 * It reads a line of input from the user, and if the user typed "exit", it exits the program
 *
 * @param input The input string.
 *
 * @return a boolean value indicating whether reading the input was succesfull or not
 */
bool read_input(char *input)
{
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL)
    {
        return false;
    }
    input[strcspn(input, "\n")] = '\0';
    return true;
}

/**
 * It removes all the double quotes from the input string
 *
 * @param input The input string
 */
void parse_input(char *input)
{
    char *p = input;
    char *q = input;
    while (*q != '\0')
    {
        if (*q != '\"')
        {
            *p = *q;
            p++;
        }
        q++;
    }
    *p = '\0';
    execute_commands(input);
}

/**
 * It reads a line of input from the user, and then parses it
 *
 * @return The exit status of the program.
 */
int main()
{
    char input[MAX_INPUT_SIZE];
    signal(SIGINT, ctrl_c_handler);

    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    while (true)
    {

        if (!read_input(input))
        {
            break;
        }

        parse_input(input);
    }

    return EXIT_SUCCESS;
}
