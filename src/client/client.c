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
int main(int argc, char const *argv[])
{
	if (argc != 3) {
		printf("Usage: %s IPAddress PortNumber\n", argv[0]);
		exit(1);
	}
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Create client socket
    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(argv[2]));
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server.\n");

    while (1) {
        printf("Enter a message (or 'exit' to quit): ");
        fgets(buffer, sizeof(buffer), stdin);

        // Remove the newline character from the input
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Send the message to the server
        send(clientSocket, buffer, strlen(buffer), 0);

        // Exit the loop if the user entered "exit"
        if (strcmp(buffer, "exit") == 0) {
            break;
        }

        // Receive and print the echoed message from the server
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesRead <= 0) {
            perror("Connection closed or error");
            break;
        }

        printf("Server response: %s\n", buffer);
    }

    // Close the socket
    close(clientSocket);

    return 0;
}
