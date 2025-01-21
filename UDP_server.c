#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void main() {
    struct sockaddr_in client, server; // Structures to store client and server addresses
    int sockfd, n, bindsts; // Socket file descriptor and size variable
    char rBuf[100] = "", sBuf[100] = ""; // Buffers for receiving and sending data
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Create a socket (UDP)
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 

    // Initialize server address structure
    server.sin_family = AF_INET; // IPv4
    server.sin_port = 2025; // Port number
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address

    bindsts = bind(sockfd, (struct sockaddr *)&server, sizeof server); // Bind the socket to the server address
    if (bindsts != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n");

    printf("\nServer ready, waiting for client….\n"); // Indicate that server is ready
    n = sizeof client; // Get the size of the client address structure
    
    while(1) { //infinite loop for communication
    
        recvfrom(sockfd, rBuf, sizeof rBuf, 0, (struct sockaddr *)&client, &n); // Receive data from the client
        printf("\nClient:%s", rBuf); // Print the received data
        
        if(strcmp(rBuf, "end") == 0) // If client sends "end", break the loop
            break;
        
        printf("\nServer:"); // Prompt for server input
        gets(sBuf); // Get server input
        
        sendto(sockfd, sBuf, sizeof sBuf, 0, (struct sockaddr *)&client, n); // Send data to the client
        
        if(strcmp(sBuf, "end") == 0) // If server sends "end", break the loop
            break;
    }
    
    close(sockfd); // Close the socket
}

