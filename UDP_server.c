#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 100
#define PORT 2025

int main() {
    struct sockaddr_in client, server;
    int sockfd, bind_status;
    socklen_t client_len;
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
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Bind socket to address
    bind_status = bind(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (bind_status != 0) {
        perror("Socket bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Socket successfully bound to port %d\n", PORT);

    printf("Server ready, waiting for client messages...\n");
    client_len = sizeof(client);

    while(1) {
        // Receive message from client
        memset(rBuf, 0, BUFFER_SIZE);
        ssize_t recv_len = recvfrom(sockfd, rBuf, BUFFER_SIZE, 0, 
                                   (struct sockaddr *)&client, &client_len);
        
        if (recv_len <= 0) {
            perror("Receive failed");
            break;
        }
        
        printf("\nClient: %s", rBuf);
        
        // Check for termination command
        if(strcmp(rBuf, "end") == 0) {
            break;
        }
        
        // Get server response
        printf("\nServer: ");
        memset(sBuf, 0, BUFFER_SIZE);
        if (fgets(sBuf, BUFFER_SIZE, stdin) == NULL) {
            perror("Input error");
            break;
        }
        
        // Remove newline character if present
        sBuf[strcspn(sBuf, "\n")] = '\0';
        
        // Send response to client
        if (sendto(sockfd, sBuf, strlen(sBuf)+1, 0, 
                 (struct sockaddr *)&client, client_len) <= 0) {
            perror("Send failed");
            break;
        }
        
        // Check for termination command
        if(strcmp(sBuf, "end") == 0) {
            break;
        }
    }
    
    close(sockfd);
    printf("Server shutting down\n");
    return 0;
}
