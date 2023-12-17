#include "../include/shared/utility.h"
 
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
void util_substring(const char *string, char *buffer, int pos, int len)
{
    char * temp = (char *) malloc((len + 1)*sizeof(char));
    strcpy(temp, string + pos);
    temp[len] = '\0';  // Null-terminate the string
    strcpy(buffer, temp);
}