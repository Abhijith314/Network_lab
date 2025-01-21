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
    struct sockaddr_in server; // Structure to store server address
    int sockfd, n; // Socket file descriptor and size variable
    char rBuf[100] = "", sBuf[100] = ""; // Buffers for receiving and sending data
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0); // Create a socket (UDP)
                                          // SOCK_DGRAM indicates that its UDP
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
    
    printf("\nClient ready….\n"); // Indicate that client is ready
    n = sizeof server; // Get the size of the server address structure
    
    while(1) { //infinite loop for communication
    
        printf("\nClient:"); // Prompt for user input
        gets(sBuf); // Get user input
        
        sendto(sockfd, sBuf, sizeof sBuf, 0, (struct sockaddr *)&server, n); // Send data to the server
        
        if(strcmp(sBuf, "end") == 0) // If user enters "end", break the loop
            break;
        
        recvfrom(sockfd, rBuf, sizeof rBuf, 0, (struct sockaddr*)&server, &n); // Receive data from the server
        printf("\nServer:%s", rBuf); // Print the received data
        
        if(strcmp(rBuf, "end") == 0) // If server sends "end", break the loop
            break;
    }
    
    close(sockfd); // Close the socket
}

