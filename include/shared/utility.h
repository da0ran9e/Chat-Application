#ifndef ULTILITY_H
#define ULTILITY_H

// get current timestamp as string (format: YYYY-mm-dd HH:MM:SS)
void util_get_time(char * timeString);
// get a substring from a binary string
void util_substring(const char *string, char *buffer, int pos, int len);

#endif