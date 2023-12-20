#ifndef ULTILITY_H
#define ULTILITY_H

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// get current timestamp as string (format: YYYY-mm-dd HH:MM:SS)
void util_get_time(char * timeString);
// get a substring from a binary string
void util_get_substring(const char *string, char *buffer, int pos, int len);
// Function to convert an integer to a string
char* util_int_to_str(int value);

#endif