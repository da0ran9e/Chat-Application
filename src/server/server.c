#include "../../include/server.h"
#include "../../include/shared/constants.h"
void handleClient(int clientSocket) {
    char buffer[BUFFER];
    ssize_t bytesRead;

    // Receive message from the client
    bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        perror("Error receiving message from client");
        close(clientSocket);
        return;
    }

    // Process the received message (modify as needed)
    buffer[bytesRead] = '\0'; // Ensure null-terminated string
    printf("Received message from client: %s\n", buffer);

    // Send a response back to the client
    const char* responseMessage = "Response from the server!";
    ssize_t bytesSent = send(clientSocket, responseMessage, strlen(responseMessage), 0);
    if (bytesSent < 0) {
        perror("Error sending response to client");
    }

    close(clientSocket);
}

void initializeServer() {
    int serverSocket;
    struct sockaddr_in serverAddr;

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error creating server socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address struct
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    // Bind the socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Error binding server socket");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        perror("Error listening for connections");
        close(serverSocket);
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);
}

int main() {
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    initializeServer();

    while (1) {
        // Accept a client connection
        if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen)) == -1) {
            perror("Error accepting client connection");
            continue; // Continue to the next iteration of the loop
        }

        // Handle the client in a separate function
        handleClient(clientSocket);
    }

    // Close the server socket (this part is unreachable in this example)
    close(serverSocket);

    return 0;
}
