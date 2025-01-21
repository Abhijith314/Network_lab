#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main() {
    struct sockaddr_in client, server; // Structures to store client and server addresses
    int sockfd, n, confd, bindsts, lstnsts; // Socket file descriptors and size variable
    char rBuf[100] = "", sBuf[100] = ""; // Buffers for receiving and sending data
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create a socket
    if (sockfd == -1) { 
        printf("socket creation failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully created..\n"); 
  
  
    // Initialize server address structure
    server.sin_family = AF_INET; // IPv4
    server.sin_port = 6060; // Port number
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address
    
    bindsts = bind(sockfd, (struct sockaddr *)&server, sizeof(server)); // Bind the socket to the server address
    
    if (bindsts != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
  

    lstnsts = listen(sockfd, 1); // Listen for incoming connections, since TCP is connection oriented
    if (lstnsts != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 

    printf("\nServer ready, waiting for client….\n"); // Indicate that server is ready
    confd = accept(sockfd, (struct sockaddr *)&client, &n); // Accept a connection from a client
        
    while(1) { //infinite loop for communication
    
        n = sizeof(client); // Get the size of the client address structure
        recv(confd, rBuf, sizeof(rBuf), 0); // Receive data from the client
        printf("\nClient: %s", rBuf); // Print the received data
        
        if(strcmp(rBuf, "end") == 0) // If client sends "end", break the loop
            break;
        
        printf("\nServer: "); // Prompt for server input
        gets(sBuf); // Get server input
        
        send(confd, sBuf, sizeof(sBuf), 0); // Send data to the client
        
        if(strcmp(sBuf, "end") == 0) // If server sends "end", break the loop
            break;
        
        printf("\n"); // Add a newline for clarity
    }
    
    close(confd); // Close the connection with the client
    close(sockfd); // Close the server socket
    return 0;
}

