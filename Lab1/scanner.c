#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "scanner.h"

/**
 * It takes a character and check whether this is a whitespace
 * 
 * @param c the character to check
 * 
 * @return A boolean indicating whether it is a whitespace or not
 */
bool is_whitespace(char c) {
    return isspace(c);
}

/**
 * It returns true if the character is a delimiter, and false otherwise
 * 
 * @param c The character to check
 * 
 * @return A boolean indicating whether the character is a delimiter or not
 */
bool is_delimiter(char c) {
    return 
    c == '|' || 
    c == '<' || 
    c == '>' || 
    strcmp(&c,"&&") == 0 || 
    strcmp(&c,"||") == 0 || 
    strcmp(&c,";") == 0 ;
}

/**
 * It takes a string and returns a pointer to the same string, but with all leading and trailing
 * whitespace removed
 * 
 * @param str The string to trim.
 * 
 * @return A pointer to the first non-whitespace character in the string.
 */
char *trim_whitespace(char *str) {
    while (is_whitespace(*str)) {
        str++;
    }
    if (*str == '\0') {
        return str;
    }
    char *end = str + strlen(str) - 1;
    while (end > str && is_whitespace(*end)) {
        end--;
    }
    *(end + 1) = '\0';
    return str;
}
