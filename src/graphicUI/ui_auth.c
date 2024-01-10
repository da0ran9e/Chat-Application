#include "../../include/webview/webview.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define UNUSED(x) (void)x

typedef struct {
  webview_t w;
  unsigned int count;
} context_t;

// Function to concatenate content from multiple files into a single string
const char* concatenateFiles(const char* filenames[], int numFiles) {
    // Calculate the total size needed for the concatenated content
    long totalSize = 1; // Start with 1 for the null terminator
    for (int i = 0; i < numFiles; ++i) {
        FILE* file = fopen(filenames[i], "r");
        if (file == NULL) {
            perror("Error opening file");
            return NULL;
        }

        fseek(file, 0, SEEK_END);
        totalSize += ftell(file);
        fclose(file);
    }

    // Allocate memory for the concatenated content
    char* concatenatedContent = (char*)malloc(totalSize);
    if (concatenatedContent == NULL) {
        perror("Memory allocation error");
        return NULL;
    }

    // Concatenate content from each file
    long offset = 0;
    for (int i = 0; i < numFiles; ++i) {
        FILE* file = fopen(filenames[i], "r");
        if (file == NULL) {
            perror("Error opening file");
            free(concatenatedContent);
            return NULL;
        }

        fseek(file, 0, SEEK_END);
        long size = ftell(file);
        fseek(file, 0, SEEK_SET);

        fread(concatenatedContent + offset, 1, size, file);
        offset += size;

        fclose(file);
    }

    // Add null terminator at the end
    concatenatedContent[offset] = '\0';

    return concatenatedContent;
}

// Function to parse parameters from the given format
int parseReq(const char* req, char(*params)[50]) {
    int count = 0;

    // Check if the input string starts with "[" and ends with "]"
    if (req[0] == '[' && req[strlen(req) - 1] == ']') {
        // Copy the string without "[" and "]"
        char reqCopy[255]; // Adjust the size as needed
        strncpy(reqCopy, req + 1, strlen(req) - 2);
        reqCopy[strlen(req) - 2] = '\0';

        // Tokenize the string using ","
        char* token = strtok(reqCopy, ",");
        while (token != NULL && count < 50) {
            // Copy each parameter to the params array
            strncpy(params[count], token + 1, strlen(token) - 2);
            params[count][strlen(token) - 2] = '\0';

            token = strtok(NULL, ",");
            count++;
        }
    }

    return count;
}

void u_login(const char *seq, const char *req, void *arg) {
  context_t *context = (context_t *)arg;
  char p[2][50];
  int count = parseReq(req, p);

    printf("Input: %s", req);
    printf("Number of parameters: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("Parameter %d: %s\n", i + 1, p[i]);
    }

  webview_return(context->w, seq, 0, "0");
}

int main() {
  const char* filenames[] = {"src/auth.cui", "src/tailwind.cui"}; // Replace with your file names
  int numFiles = sizeof(filenames) / sizeof(filenames[0]);

  const char* concatenatedContent = concatenateFiles(filenames, numFiles);

  webview_t w = webview_create(0, NULL);
  context_t context = {.w = w, .count = 0};
  webview_set_title(w, "Authentication");
  webview_set_size(w, 350, 700, WEBVIEW_HINT_NONE);

  webview_bind(w, "u_login", u_login, &context);

  webview_set_html(w, concatenatedContent);
  webview_run(w);
  webview_destroy(w);
  return 0;
}
