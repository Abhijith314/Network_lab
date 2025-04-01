#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 100

int main() {
    struct sockaddr_in server; // Structure to store server address
    int client_socket, connsts; // Socket file descriptor
    char rBuf[BUFFER_SIZE] = "", sBuf[BUFFER_SIZE] = ""; // Buffers for receiving and sending data
    
    // Create a TCP socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created\n");
    
    // Configure server address
    server.sin_family = AF_INET; // IPv4
    server.sin_port = htons(6060); // Port number (using htons for correct byte order)
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost IP
    
    printf("Client ready, connecting to server...\n");
    
    // Connect to the server
    connsts = connect(client_socket, (struct sockaddr *)&server, sizeof(server));
    if (connsts != 0) {
        perror("Connection failed");
        close(client_socket);
        exit(EXIT_FAILURE);
    }
    printf("Connected to server at 127.0.0.1:6060\n");

    while (1) {
        // Get user input
        printf("Client: ");
        memset(sBuf, 0, BUFFER_SIZE); // Clear buffer
        if (fgets(sBuf, BUFFER_SIZE, stdin) == NULL) {
            perror("Input error");
            break;
        }
        
        // Remove newline character if present
        sBuf[strcspn(sBuf, "\n")] = '\0';
        
        // Send data to server
        if (send(client_socket, sBuf, strlen(sBuf)+1, 0) <= 0) { // +1 to include null terminator
            perror("Send failed");
            break;
        }
        
        if (strcmp(sBuf, "end") == 0) {
            break;
        }
        
        // Receive server response
        memset(rBuf, 0, BUFFER_SIZE); // Clear buffer
        if (recv(client_socket, rBuf, BUFFER_SIZE, 0) <= 0) {
            perror("Receive failed or connection closed");
            break;
        }
        printf("Server: %s\n", rBuf);
        
        if (strcmp(rBuf, "end") == 0) {
            break;
        }
    }
    
    printf("Closing connection\n");
    close(client_socket);
    return 0;
}
