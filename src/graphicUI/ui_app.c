#include "../../include/webview/webview.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define UNUSED(x) (void)x

int thread_create(void *(*start_routine)(void *), void *arg) {
    pthread_t thread;
    int error = pthread_create(&thread, NULL, start_routine, arg);
    if (error == 0) {
        pthread_detach(thread);
        return 0;
    }
    return -1;

}

void thread_sleep(int seconds) {
  sleep(seconds);
}

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

int main() {
  const char* filenames[] = {"src/layout.cui", "src/tailwind.cui", "src/app.cui", "src/style.cui"}; // Replace with your file names
  int numFiles = sizeof(filenames) / sizeof(filenames[0]);

  const char* application = concatenateFiles(filenames, numFiles);

  webview_t w = webview_create(0, NULL);
  //context_t context = {.w = w, .count = 0};
  webview_set_title(w, "App");
  webview_set_size(w, 1300, 700, WEBVIEW_HINT_NONE);

  webview_set_html(w, application);
  webview_run(w);
  webview_destroy(w);
  return 0;
}
