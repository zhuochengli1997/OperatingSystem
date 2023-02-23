#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "scanner.h"

bool is_whitespace(char c) {
    return isspace(c);
}

bool is_delimiter(char c) {
    return 
    c == '|' || 
    c == '<' || 
    c == '>' || 
    strcmp(&c,"&&") == 0 || 
    strcmp(&c,"||") == 0 || 
    strcmp(&c,";") == 0 ;
}

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
