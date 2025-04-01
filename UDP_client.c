#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 100
#define SERVER_PORT 2025
#define SERVER_IP "127.0.0.1"

int main() {
    struct sockaddr_in server;
    int sockfd;
    socklen_t server_len;
    char rBuf[BUFFER_SIZE] = "";
    char sBuf[BUFFER_SIZE] = "";
    
    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully created\n");

    // Configure server address
    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(SERVER_PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    printf("Client ready to communicate with server %s:%d\n", SERVER_IP, SERVER_PORT);
    server_len = sizeof(server);

    while(1) {
        // Get user input
        printf("\nClient: ");
        memset(sBuf, 0, BUFFER_SIZE);
        if (fgets(sBuf, BUFFER_SIZE, stdin) == NULL) {
            perror("Input error");
            break;
        }
        
        // Remove newline character
        sBuf[strcspn(sBuf, "\n")] = '\0';
        
        // Send message to server
        if (sendto(sockfd, sBuf, strlen(sBuf)+1, 0, 
                 (struct sockaddr *)&server, server_len) <= 0) {
            perror("Send failed");
            break;
        }
        
        if(strcmp(sBuf, "end") == 0) {
            break;
        }
        
        // Receive response from server
        memset(rBuf, 0, BUFFER_SIZE);
        if (recvfrom(sockfd, rBuf, BUFFER_SIZE, 0, 
                    (struct sockaddr *)&server, &server_len) <= 0) {
            perror("Receive failed");
            break;
        }
        printf("Server: %s", rBuf);
        
        if(strcmp(rBuf, "end") == 0) {
            break;
        }
    }
    
    close(sockfd);
    printf("Client shutting down\n");
    return 0;
}
