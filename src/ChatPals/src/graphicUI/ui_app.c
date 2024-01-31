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
    long totalSize = 1;
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
    char* concatenatedContent = (char*)malloc(totalSize);
    if (concatenatedContent == NULL) {
        perror("Memory allocation error");
        return NULL;
    }
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
    concatenatedContent[offset] = '\0';
    return concatenatedContent;
}

// Function to parse parameters from the given format
int parseReq(const char* req, char(*params)[50]) {
    int count = 0;
    if (req[0] == '[' && req[strlen(req) - 1] == ']') {
        char reqCopy[255]; 
        strncpy(reqCopy, req + 1, strlen(req) - 2);
        reqCopy[strlen(req) - 2] = '\0';
        char* token = strtok(reqCopy, ",");
        while (token != NULL && count < 50) {
            strncpy(params[count], token + 1, strlen(token) - 2);
            params[count][strlen(token) - 2] = '\0';
            token = strtok(NULL, ",");
            count++;
        }
    }
    return count;
}

/*Application*/

void a_update_data(const char *seq, const char *req, void *arg) {
    context_t *context = (context_t *)arg;
    char p[1][50];
    int count = parseReq(req, p);
    /*code go here*/
    printf("Input: %s", req);
    printf("Number of parameters: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("Parameter %d: %s\n", i + 1, p[i]);
    }

  webview_return(context->w, seq, 0, "OK");
}

void a_send_message(const char *seq, const char *req, void *arg) {
    context_t *context = (context_t *)arg;
    char p[1][50];
    int count = parseReq(req, p);
    /*code go here*/
    printf("Input: %s", req);
    printf("Number of parameters: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("Parameter %d: %s\n", i + 1, p[i]);
    }

  webview_return(context->w, seq, 0, "OK");
}

void a_create_room(const char *seq, const char *req, void *arg) {
    context_t *context = (context_t *)arg;
    char p[1][50];
    int count = parseReq(req, p);
    /*code go here*/
    printf("Input: %s", req);
    printf("Number of parameters: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("Parameter %d: %s\n", i + 1, p[i]);
    }

  webview_return(context->w, seq, 0, "OK");
}

void a_add_member(const char *seq, const char *req, void *arg) {
    context_t *context = (context_t *)arg;
    char p[1][50];
    int count = parseReq(req, p);
    /*code go here*/
    printf("Input: %s", req);
    printf("Number of parameters: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("Parameter %d: %s\n", i + 1, p[i]);
    }

  webview_return(context->w, seq, 0, "OK");
}

void a_remove_member(const char *seq, const char *req, void *arg) {
    context_t *context = (context_t *)arg;
    char p[1][50];
    int count = parseReq(req, p);
    /*code go here*/
    printf("Input: %s", req);
    printf("Number of parameters: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("Parameter %d: %s\n", i + 1, p[i]);
    }

  webview_return(context->w, seq, 0, "OK");
}

void a_send_request(const char *seq, const char *req, void *arg) {
    context_t *context = (context_t *)arg;
    char p[1][50];
    int count = parseReq(req, p);
    /*code go here*/
    printf("Input: %s", req);
    printf("Number of parameters: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("Parameter %d: %s\n", i + 1, p[i]);
    }

  webview_return(context->w, seq, 0, "OK");
}

void a_response_request(const char *seq, const char *req, void *arg) {
    context_t *context = (context_t *)arg;
    char p[1][50];
    int count = parseReq(req, p);
    /*code go here*/
    printf("Input: %s", req);
    printf("Number of parameters: %d\n", count);
    for (int i = 0; i < count; i++) {
        printf("Parameter %d: %s\n", i + 1, p[i]);
    }

  webview_return(context->w, seq, 0, "OK");
}

int main() {
  const char* filenames[] = {"src/layout.cui", "src/tailwind.cui", "src/app.cui", "src/style.cui"}; // Replace with your file names
  int numFiles = sizeof(filenames) / sizeof(filenames[0]);

  const char* application = concatenateFiles(filenames, numFiles);

  webview_t w = webview_create(0, NULL);
    context_t context = {.w = w, .count = 0};   
  webview_set_title(w, "App");
  webview_set_size(w, 1300, 700, WEBVIEW_HINT_NONE);

    webview_bind(w, "a_update_data", a_update_data, &context);
    webview_bind(w, "a_send_message", a_send_message, &context);
    webview_bind(w, "a_create_room", a_create_room, &context);
    webview_bind(w, "a_add_member", a_add_member, &context);
    webview_bind(w, "a_remove_member", a_remove_member, &context);
    webview_bind(w, "a_send_request", a_send_request, &context);
    webview_bind(w, "a_response_request", a_response_request, &context);

  webview_set_html(w, application);
  webview_run(w);
  webview_destroy(w);
  return 0;
}
