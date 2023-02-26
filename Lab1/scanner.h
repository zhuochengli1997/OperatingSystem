#ifndef SCANNER_H
#define SCANNER_H

#include <stdbool.h>

bool is_whitespace(char c);

bool is_delimiter(char c);

char *trim_whitespace(char *str);

#endif
