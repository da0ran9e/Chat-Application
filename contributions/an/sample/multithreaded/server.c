#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>

#define MAXLINE 4096
#define SERV_PORT 3000
#define LISTENQ 8
#define MAX_THREADS 10

// Structure to pass arguments to the thread
struct ThreadArgs {
    int connfd;
};

// Function to handle I/O asynchronously in a thread
void *handleConnection(void *args) {
    struct ThreadArgs *threadArgs = (struct ThreadArgs *)args;
    int connfd = threadArgs->connfd;
    char buf[MAXLINE];
    int n;

    while ((n = recv(connfd, buf, MAXLINE, 0)) > 0) {
        printf("String received from and resent to the client:\n");
        puts(buf);
        send(connfd, buf, n, 0);
    }

    if (n < 0) {
        perror("Read error");
        exit(1);
    }

    close(connfd);
    free(threadArgs); // Free the memory allocated for thread arguments
    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    int listenfd, connfd;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    // Creation of the socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    // Preparation of the socket address
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);

    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    printf("%s\n", "Server running...waiting for connections.");

    pthread_t threads[MAX_THREADS];
    int threadCount = 0;

    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        printf("%s\n", "Received request...");

        // Create thread arguments
        struct ThreadArgs *threadArgs = (struct ThreadArgs *)malloc(sizeof(struct ThreadArgs));
        threadArgs->connfd = connfd;

        // Create a new thread to handle the connection
        pthread_create(&threads[threadCount], NULL, handleConnection, (void *)threadArgs);

        // Increment thread count and check if it exceeds the maximum
        threadCount++;
        if (threadCount >= MAX_THREADS) {
            fprintf(stderr, "Error: Maximum number of threads reached.\n");
            break;
        }
    }

    // Wait for all threads to finish
    for (int i = 0; i < threadCount; i++) {
        pthread_join(threads[i], NULL);
    }

    // Close listening socket
    close(listenfd);

    return 0;
}
