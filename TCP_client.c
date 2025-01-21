#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main() {
    struct sockaddr_in server; // Structure to store server address
    int client_socket, connsts; // Socket file descriptor
    char rBuf[100] = "", sBuf[100] = ""; // Buffers for receiving and sending data
    
    client_socket = socket(AF_INET, SOCK_STREAM, 0); // Create a socket(address_family,socket_type, protocol=0 (Default))
                                                    // SOCK_STREAM indicates that its TCP
    if (client_socket == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    
    // Initialize server address structure
    server.sin_family = AF_INET; // AF_INET is used for IPv4 addresses
    server.sin_port = 6060; // Port number
    server.sin_addr.s_addr = inet_addr("127.0.0.1"); // IP address
    
    printf("\nClient ready....\n"); // Indicate that client is ready
    
    connsts = connect(client_socket, (struct sockaddr *)&server, sizeof(server)); // Since TCP is connection oriented we need to Connect
        
    if (connsts != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");


    while (1) { //infinite loop for communication
    
        printf("\nClient: "); // Prompt for user input
        gets(sBuf); // Get user input
        
        send(client_socket, sBuf, sizeof(sBuf), 0); // Send data to the server
        
        if (strcmp(sBuf, "end") == 0) // If user enters "end", break the loop
            break;
        
        recv(client_socket, rBuf, sizeof(rBuf), 0); // Receive data from the server
        //strrev(rBuf);
        printf("\nServer: %s", rBuf); // Print the received data
        
        if (strcmp(rBuf, "end") == 0) // If server sends "end", break the loop
            break;
        
        printf("\n"); // Add a newline for clarity
    }
    
    close(client_socket); // Close the socket
    return 0;
}
