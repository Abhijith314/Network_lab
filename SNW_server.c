#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int main() {
    // Socket and network variables
    int sockfd, len;                    // Socket file descriptor and length variable
    int frames[100], n, p, ack;         // Frame array, frame count, current frame, acknowledgment
    struct sockaddr_in servaddr, cliaddr; // Server and client address structures

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    printf("\nwaiting for client...\n");

    // Initialize server and client address structures
    memset(&servaddr, 0, sizeof(servaddr));  // Clear server address structure
    memset(&cliaddr, 0, sizeof(cliaddr));    // Clear client address structure

    // Configure server address
    servaddr.sin_family = AF_INET;           // IPv4 address family
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections on any interface
    servaddr.sin_port = htons(6060);         // Listen on port 6060

    // Bind socket to server address
    if (bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    len = sizeof(cliaddr);  // Get size of client address structure

    // Wait for initial connection message from client
    recvfrom(sockfd, &n, sizeof(n), 0, (struct sockaddr *)&cliaddr, &len);
    printf("\nClient connected successfully.\n");

    // Wait for total number of frames from client
    printf("\nwaiting for total number of frames...\n");
    recvfrom(sockfd, &n, sizeof(n), 0, (struct sockaddr *)&cliaddr, &len);

    // Main communication loop
    while (1) {
        // Receive frame number from client
        recvfrom(sockfd, &p, sizeof(n), 0, (struct sockaddr *)&cliaddr, &len);

        // Check for termination signal (p = -99)
        if (p == -99) {
            printf("\nReceived termination signal. Closing server.\n");
            close(sockfd);
            return 0;
        }

        printf("\nReceived frame-%d ", p);
        
        // Get acknowledgment decision from user
        printf("\nEnter 1 for +ve ack and -1 for -ve ack.\n");
        scanf("%d", &ack);

        // Send acknowledgment back to client
        sendto(sockfd, &ack, sizeof(n), 0, (struct sockaddr *)&cliaddr, sizeof(cliaddr));
    }

    close(sockfd);  // Close socket (though we exit before this in normal operation)
    return 0;
}
