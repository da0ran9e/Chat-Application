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

// Function to parse username and password from the given format
char** parseCredentials(const char* input) {
    // Check for null input
    if (input == NULL) {
        fprintf(stderr, "Error: Null input\n");
        return NULL;
    }

    // Check for the correct format
    if (input[0] != '[' || input[strlen(input) - 1] != ']') {
        fprintf(stderr, "Error: Invalid format\n");
        return NULL;
    }

    // Count the number of pairs
    int numPairs = 1;
    for (int i = 0; input[i] != '\0'; ++i) {
        if (input[i] == ',' && input[i + 1] == '"') {
            numPairs++;
        }
    }

    // Allocate memory for the 2D array
    char** credentials = (char**)malloc(numPairs * sizeof(char*));
    if (credentials == NULL) {
        perror("Memory allocation error");
        return NULL;
    }

    // Initialize each element to NULL
    for (int i = 0; i < numPairs; ++i) {
        credentials[i] = NULL;
    }

    // Parse the input and fill the 2D array
    int pairIndex = 0;
    for (int i = 1; input[i] != '\0' && input[i] != ']'; ++i) {
        if (input[i] == '"' && input[i + 1] == '"') {
            // Skip double quotes
            i++;
        } else if (input[i] == '"' && input[i - 1] == ',') {
            // Start of a pair, find the end of the username
            int start = i + 1;
            while (input[i + 1] != '"' && input[i + 1] != '\0') {
                i++;
            }
            int end = i;
            
            // Allocate memory for the username and copy it
            int usernameLength = end - start + 1;
            credentials[pairIndex] = (char*)malloc(usernameLength + 1);
            strncpy(credentials[pairIndex], input + start, usernameLength);
            credentials[pairIndex][usernameLength] = '\0';
            pairIndex++;
        }
    }

    return credentials;
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

  char userInfor = parseCredentials(params->req)
  //sscanf(params->req,"[\"%s\",\"%s\"]", username, password);
  printf("Login for %s by %s\n", userInfor[0], userInfor[1]);
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
