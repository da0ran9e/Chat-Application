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
const char* readFromFile(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* content = (char*)malloc(size + 1);
    fread(content, 1, size, file);
    content[size] = '\0';

    fclose(file);

    return content;
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
  char* main = readFromFile("auth.cui");
  char* tailwind = readFromFile("tailwind.cui");
  strcat(main, tailwind);

  webview_t w = webview_create(0, NULL);
  context_t context = {.w = w, .count = 0};
  webview_set_title(w, "Bind Example");
  webview_set_size(w, 700, 500, WEBVIEW_HINT_NONE);

  // A binding that increments a value and immediately returns the new value.
  webview_bind(w, "increment", increment, &context);

  // An binding that creates a new thread and returns the result at a later time.
  webview_bind(w, "compute", compute, &context);

  webview_set_html(w, main);
  webview_run(w);
  webview_destroy(w);
  return 0;
}
