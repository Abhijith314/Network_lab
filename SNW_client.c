#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

int main() {
    // Socket and network variables
    int sockfd;                      // Socket file descriptor
    int n;                           // Total number of frames to send
    int i;                           // Loop counter (current frame number)
    int len;                         // Length of address structure
    int ack;                         // Acknowledgment from server (-1 or 1)
    struct sockaddr_in servaddr;     // Server address structure

    printf("\nSearching for server...\n");
    
    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Initialize server address structure
    memset(&servaddr, 0, sizeof(servaddr));  // Clear the structure
    servaddr.sin_family = AF_INET;           // IPv4 address family
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY); // Send to any interface
    servaddr.sin_port = htons(6060);         // Server port 6060

    // Send initial connection message to server
    n = -1;  // Special value indicating connection request
    sendto(sockfd, &n, sizeof(n), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("\nServer connected successfully.\n");

    // Get total number of frames from user
    printf("\nEnter the total number of frames: ");
    scanf("%d", &n);
    
    // Send total frame count to server
    sendto(sockfd, &n, sizeof(n), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

    // Frame transmission loop
    for (i = 1; i <= n; i++) {
        ack = -1;  // Initialize acknowledgment to negative
        
        // Retry loop for each frame until positive ACK received
        do {
            printf("\nSending frame-%d", i);
            
            // Send current frame number to server
            sendto(sockfd, &i, sizeof(n), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
            
            printf("\nWaiting for ACK...\n");
            
            // Receive acknowledgment from server
            len = sizeof(servaddr);
            recvfrom(sockfd, &ack, sizeof(n), 0, (struct sockaddr *)&servaddr, &len);
            
            if (ack == -1) {
                printf("\nNegative ACK received. Resending frame-%d...\n", i);
            }
        } while (ack == -1);  // Continue resending until positive ACK received
    }

    // Send termination signal to server
    n = -99;  // Special value indicating end of transmission
    sendto(sockfd, &n, sizeof(n), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr));
    printf("\nSuccessfully sent all %d frames.\n", n);

    close(sockfd);  // Close the socket
    return 0;
}
