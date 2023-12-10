#include "../../include/server.h"
#include "../../include/shared/constants.h"
// #include "../../include/server/log.h"

ssize_t receiveMessage(int clientSocket, char *buf) {
    char buffer[BUFFER];
    ssize_t bytesRead;

    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        perror("Error receiving message from client");
        close(clientSocket);
        return -1;
    }

    buffer[bytesRead] = '\0';
    printf("Received message from client: %s\n", buffer);

    close(clientSocket);

    return bytesRead;
}

ssize_t sendMessage(int clientSocket, const void *buf, size_t len) {
    size_t totalSent = 0;

    while (totalSent < len) {
        ssize_t sent = send(clientSocket, buf + totalSent, len - totalSent, 0);

        if (sent == -1) {
            perror("Error in send");
            return -1;
        } else if (sent == 0) {
            fprintf(stderr, "Client %d is not online!\n", clientSocket);
            return 0; 
        }

        totalSent += sent;
    }

    return totalSent;
}

void initializeServer(int serverSocket) {
    struct sockaddr_in serverAddr;

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding server socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) == -1) {
        perror("Error listening for connections");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);
}

void runServer(int  serverSocket){
    struct sockaddr_in clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

	int clientSocket, sockfd;
	int nready;
    while (1) {
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
            perror("Error accepting client connection");
            continue; 
        }

        char buffer[1024]; 
        int received = receiveMessage(clientSocket, buffer);
        printf("Received: %d, content: %s from ", received, buffer);

        int sent = sendMessage(clientSocket, "response!", 9);
        printf(" sent response: %d\n",sent);
    }
}

int main(int argc, char *argv[]){
	if (argc != 2) {
		printf("Usage: %s PortNumber\n", argv[0]);
		exit(1);
	}
    
    int serverSocket = atoi(argv[1]);
    int clientSocket = 0;

    initializeServer(serverSocket);

    runServer(serverSocket);

    // serverLog(RUNNING, serverSocket);

    close(serverSocket);

    return 0;
}
