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

typedef struct {
    char *username;
    char *password;
} UserInfo;

// Function to parse username and password from the given format
UserInfo parseUserInfo(const char *input) {
    UserInfo userInfo = {NULL, NULL};

    // Find the first occurrence of double quotes
    const char *firstQuote = strchr(input, '"');
    if (firstQuote == NULL) {
        fprintf(stderr, "Invalid input format\n");
        return userInfo;
    }

    // Find the second occurrence of double quotes
    const char *secondQuote = strchr(firstQuote + 1, '"');
    if (secondQuote == NULL) {
        fprintf(stderr, "Invalid input format\n");
        return userInfo;
    }

    // Calculate the length of the username
    size_t usernameLength = secondQuote - firstQuote - 1;

    // Allocate memory for the username and copy the content
    userInfo.username = (char *)malloc(usernameLength + 1);
    strncpy(userInfo.username, firstQuote + 1, usernameLength);
    userInfo.username[usernameLength] = '\0';

    // Find the third occurrence of double quotes
    const char *thirdQuote = strchr(secondQuote + 1, '"');
    if (thirdQuote == NULL) {
        fprintf(stderr, "Invalid input format\n");
        free(userInfo.username);
        return userInfo;
    }

    // Calculate the length of the password
    size_t passwordLength = thirdQuote - secondQuote - 1;

    // Allocate memory for the password and copy the content
    userInfo.password = (char *)malloc(passwordLength + 1);
    strncpy(userInfo.password, secondQuote + 1, passwordLength);
    userInfo.password[passwordLength] = '\0';

    return userInfo;
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

  UserInfo us = parseUserInfo(params->req);
  //sscanf(params->req,"[\"%s\",\"%s\"]", username, password);
  printf("Login for %s by %s\n", us.username, us.password);
  //["username","password"]
  // auto username = webview::detail::json_parse(params->req, "", 0);
	// auto password = webview::detail::json_parse(params->req, "", 1);
  // thread_sleep(1);
  // if(atoi(username[0])==atoi(password[0])){
  //   webview_return(params->w, params->seq, 0, "1");
  // }
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
  webview_set_size(w, 450, 800, WEBVIEW_HINT_NONE);

  webview_bind(w, "u_login", u_login, &context);

  webview_set_html(w, concatenatedContent);
  webview_run(w);
  webview_destroy(w);
  return 0;
}
