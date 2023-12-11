#include "../../include/client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

void initializeClient(char *address, int port){
	int clientSocket;
	struct sockaddr_in serverAddr; /* server's address information */

	//Construct socket
	clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	
	//Specify server address
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = inet_addr(address);
	
	// Request to connect server
	if(connect(clientSocket, (struct sockaddr*)&server_addr, sizeof(struct sockaddr)) < 0) {
		printf("Cannot connect to server!\n");
		close(clientSocket);
		exit(0);
	}

	return clientSocket;
}

ssize_t send_message (int socket, char *message){
    ssize_t bytesSent = send(clientSocket, message, strlen(message), 0);
	if(bytesSent <= 0) {
		perror("Error: ");
		return -1;
	}

	return bytesSent;
}

ssize_t receive_message (int socket, char *buffer){
	ssize_t bytesReceived = recv(socket, buffer, BUFFER_SIZE, 0);
	if (bytesReceived <= 0) {
		perror("NaN received: ");
		return -1;
	}

	return bytesReceived;
}

void run_client (int port, char *address){
    char message[BUFFER_SIZE];

	while (1) {
        printf("Enter a message (or 'exit' to quit): ");
        fgets(message, sizeof(message), stdin);

        // Remove the newline character from the input
        size_t len = strlen(message);
        if (len > 0 && message[len - 1] == '\n') {
            message[len - 1] = '\0';
            send_message(port, message);
        }

        // Exit the loop if the user entered "exit"
        if (strcmp(message, "exit") == 0) {
            break;
        }

        char buffer[BUFFER_SIZE];
        receive_message(port, buffer);

        printf("Server response: %s\n", buffer);
    }
}

int main(int argc, char const *argv[])
{
	if (argc != 3) {
		printf("Usage: %s IPAddress PortNumber\n", argv[0]);
		exit(1);
	}

    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    initializeClient(argv[1], atoi(argv[2]));

	run_client(argv[1], atoi(argv[2]));

    // Close the socket
    close(clientSocket);

    return 0;
}
