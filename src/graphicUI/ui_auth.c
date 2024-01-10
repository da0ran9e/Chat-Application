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

typedef struct {
  webview_t w;
  char *seq;
  char *req;
} login_thread_params_t;

login_thread_params_t *
login_thread_params_create(webview_t w, const char *seq, const char *req) {
  login_thread_params_t *params = (login_thread_params_t *)malloc(sizeof(login_thread_params_t));
  params->w = w;
  params->seq = (char *)malloc(strlen(seq) + 1);
  params->req = (char *)malloc(strlen(req) + 1);
  strcpy(params->seq, seq);
  strcpy(params->req, req);
  return params;
}

void login_thread_params_free(login_thread_params_t *p) {
  free(p->req);
  free(p->seq);
  free(p);
}

void *login_thread_proc(void *arg) {
  login_thread_params_t *params = (login_thread_params_t *)arg;

  auto username = webview::detail::json_parse(params->req, "", 0);
	auto password = webview::detail::json_parse(params->req, "", 1);
  thread_sleep(1);
  if(atoi(username[0])==atoi(password[0])){
    webview_return(params->w, params->seq, 0, "1");
  }
  webview_return(params->w, params->seq, 0, "0");
  
  login_thread_params_free(params);
  return NULL;
}

void u_login(const char *seq, const char *req, void *arg) {
  context_t *context = (context_t *)arg;
  login_thread_params_t *params = login_thread_params_create(context->w, seq, req);
  // Create a thread and forget about it for the sake of simplicity.
  if (thread_create(login_thread_proc, params) != 0) {
    login_thread_params_free(params);
  }
}

int main() {
  const char* filenames[] = {"src/auth.cui", "src/tailwind.cui"}; // Replace with your file names
  int numFiles = sizeof(filenames) / sizeof(filenames[0]);

  const char* concatenatedContent = concatenateFiles(filenames, numFiles);

  webview_t w = webview_create(0, NULL);
  context_t context = {.w = w, .count = 0};
  webview_set_title(w, "Authentication");
  webview_set_size(w, 700, 500, WEBVIEW_HINT_NONE);

  webview_bind(w, "u_login", login, &context);

  webview_set_html(w, concatenatedContent);
  webview_run(w);
  webview_destroy(w);
  return 0;
}
