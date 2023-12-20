#include "../include/shared/utility.h"

// Function to convert an integer to a string
char* util_int_to_str(int value) {
    char* str = (char*)malloc(15);  // a maximum of 12 digits for an int
    snprintf(str, 15, "%d", value);
    return str;
}

// get current timestamp as string (format: YYYY-mm-dd HH:MM:SS)
void util_get_time(char * timeString){
    time_t currentTime = time(NULL);

    if (currentTime == -1) {
        perror("Error getting current time");
        return;
    }
    struct tm* timeInfo = localtime(&currentTime);
    strftime(timeString, 20, "%Y-%m-%d %H:%M:%S", timeInfo);
}

// get a substring from a binary string
void util_get_substring(const char *string, char *buffer, int pos, int len)
{
    char * temp = (char *) malloc((len + 1)*sizeof(char));
    strcpy(temp, string + pos);
    temp[len] = '\0';  // Null-terminate the string
    strcpy(buffer, temp);
}