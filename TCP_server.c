#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 100

int main() {
    struct sockaddr_in client, server;
    int sockfd, n, confd, bindsts, lstnsts;
    char rBuf[BUFFER_SIZE] = "", sBuf[BUFFER_SIZE] = "";
    
    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) { 
        perror("Socket creation failed");
        exit(EXIT_FAILURE); 
    }
    printf("Socket successfully created\n");
  
    // Configure server address
    server.sin_family = AF_INET;
    server.sin_port = htons(6060); // Using htons for port number
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    
    // Bind socket
    bindsts = bind(sockfd, (struct sockaddr *)&server, sizeof(server));
    if (bindsts != 0) { 
        perror("Socket bind failed");
        close(sockfd);
        exit(EXIT_FAILURE); 
    }
    printf("Socket successfully bound\n");
  
    // Listen for connections
    lstnsts = listen(sockfd, 1);
    if (lstnsts != 0) { 
        perror("Listen failed");
        close(sockfd);
        exit(EXIT_FAILURE); 
    }
    printf("Server listening on port 6060\n");

    printf("Server ready, waiting for client...\n");
    n = sizeof(client);
    confd = accept(sockfd, (struct sockaddr *)&client, (socklen_t *)&n);
    if (confd < 0) {
        perror("Accept failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }
    printf("Client connected\n");
        
    while(1) {
        // Receive message from client
        memset(rBuf, 0, BUFFER_SIZE); // Clear buffer
        if (recv(confd, rBuf, BUFFER_SIZE, 0) <= 0) {
            perror("Receive failed or connection closed");
            break;
        }
        printf("Client: %s\n", rBuf);
        
        if(strcmp(rBuf, "end\n") == 0 || strcmp(rBuf, "end") == 0) {
            break;
        }
        
        // Get server response
        printf("Server: ");
        memset(sBuf, 0, BUFFER_SIZE); // Clear buffer
        if (fgets(sBuf, BUFFER_SIZE, stdin) == NULL) {
            perror("Input error");
            break;
        }
        
        // Send message to client
        if (send(confd, sBuf, strlen(sBuf), 0) <= 0) {
            perror("Send failed");
            break;
        }
        
        if(strcmp(sBuf, "end\n") == 0 || strcmp(sBuf, "end") == 0) {
            break;
        }
    }
    
    printf("Closing connection\n");
    close(confd);
    close(sockfd);
    return 0;
}
