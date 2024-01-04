#include "../../include/webview/webview.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

// Only used to suppress warnings caused by unused parameters.
#define UNUSED(x) (void)x

// Creates a thread with the given start routine and argument passed to
// the start routine. Returns 0 on success and -1 on failure.
int thread_create(void *(*start_routine)(void *), void *arg) {
    pthread_t thread;
    int error = pthread_create(&thread, NULL, start_routine, arg);
    if (error == 0) {
        pthread_detach(thread);
        return 0;
    }
    return -1;

}

// Make the current thread sleep for the given number of seconds.
void thread_sleep(int seconds) {
  sleep(seconds);
}

typedef struct {
  webview_t w;
  unsigned int count;
} context_t;

// Function to read the content of a file and return it as a string
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

void increment(const char *seq, const char *req, void *arg) {
  UNUSED(req);
  context_t *context = (context_t *)arg;

  char count_string[10] = {0};
  sprintf(count_string, "%u", ++context->count);
  char result[21] = {0};
  strcat(result, "{\"count\": ");
  strcat(result, count_string);
  strcat(result, "}");
  
  webview_return(context->w, seq, 0, result);
}

typedef struct {
  webview_t w;
  char *seq;
  char *req;
} compute_thread_params_t;

compute_thread_params_t *
compute_thread_params_create(webview_t w, const char *seq, const char *req) {
  compute_thread_params_t *params =
      (compute_thread_params_t *)malloc(sizeof(compute_thread_params_t));
  params->w = w;
  params->seq = (char *)malloc(strlen(seq) + 1);
  params->req = (char *)malloc(strlen(req) + 1);
  strcpy(params->seq, seq);
  strcpy(params->req, req);
  return params;
}

void compute_thread_params_free(compute_thread_params_t *p) {
  free(p->req);
  free(p->seq);
  free(p);
}

void *compute_thread_proc(void *arg) {
  compute_thread_params_t *params = (compute_thread_params_t *)arg;
  // Simulate load.
  thread_sleep(1);
  // Either imagine that params->req is parsed here or use your own JSON parser.
  const char *result = "42";
  webview_return(params->w, params->seq, 0, result);
  compute_thread_params_free(params);
  return NULL;
}

void compute(const char *seq, const char *req, void *arg) {
  context_t *context = (context_t *)arg;
  compute_thread_params_t *params =
      compute_thread_params_create(context->w, seq, req);
  // Create a thread and forget about it for the sake of simplicity.
  if (thread_create(compute_thread_proc, params) != 0) {
    compute_thread_params_free(params);
  }
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine,
                   int nCmdShow) {
  (void)hInst;
  (void)hPrevInst;
  (void)lpCmdLine;
  (void)nCmdShow;
#else
int main() {
#endif
  const char* filenames[] = {"auth.cui", "tailwind.cui"}; // Replace with your file names
  int numFiles = sizeof(filenames) / sizeof(filenames[0]);

  const char* concatenatedContent = concatenateFiles(filenames, numFiles);

  webview_t w = webview_create(0, NULL);
  context_t context = {.w = w, .count = 0};
  webview_set_title(w, "Authentication");
  webview_set_size(w, 700, 500, WEBVIEW_HINT_NONE);

  // A binding that increments a value and immediately returns the new value.
  webview_bind(w, "increment", increment, &context);

  // An binding that creates a new thread and returns the result at a later time.
  webview_bind(w, "compute", compute, &context);

  webview_set_html(w, concatenatedContent);
  webview_run(w);
  webview_destroy(w);
  return 0;
}
