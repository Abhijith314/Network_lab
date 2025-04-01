// SelectiveR_client_short.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

// Configuration constants
#define SERVER_IP "127.0.0.1"  // Server IP address (localhost)
#define PORT 9009              // Port number to connect to
#define TOTAL_FRAMES 5         // Total number of frames expected

/* Function to simulate frame corruption (25% probability) */
int is_faulty() {
    return (rand() % 4 > 2); // Returns 1 (true) 25% of the time
}

int main() {
    srand(time(0));  // Seed random number generator for corruption simulation
    
    // Create TCP socket
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;           // IPv4
    server.sin_port = htons(PORT);         // Port in network byte order
    server.sin_addr.s_addr = inet_addr(SERVER_IP);  // Server IP

    // Connect to server
    if (connect(client_sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("Client running... Selective Repeat ARQ\n");

    int acked_frames = 0;      // Counter for successfully received frames
    char recv_buffer[100];     // Buffer for received frames
    char ack_buffer[50];       // Buffer for acknowledgment messages

    // Main reception loop
    while (acked_frames < TOTAL_FRAMES) {
        // Clear receive buffer
        bzero(recv_buffer, sizeof(recv_buffer));
        
        // Receive frame from server
        int n = read(client_sock, recv_buffer, sizeof(recv_buffer));
        if (n <= 0) continue;  // Skip if nothing received

        // Extract frame number from message (last character)
        int frame_num = recv_buffer[strlen(recv_buffer) - 1] - '0';
        printf("Received: %s\n", recv_buffer);

        // Simulate frame corruption (25% chance)
        if (is_faulty()) {
            // Create negative acknowledgment (NACK)
            snprintf(ack_buffer, sizeof(ack_buffer), "negative ack-%d", frame_num);
            printf("Corrupted frame %d. Sending NACK.\n", frame_num);
        } else {
            // Create positive acknowledgment (ACK)
            snprintf(ack_buffer, sizeof(ack_buffer), "ack-%d", frame_num);
            printf("Frame %d OK. Sending ACK.\n", frame_num);
            acked_frames++;  // Increment counter for successful frames
        }

        // Send acknowledgment to server
        write(client_sock, ack_buffer, sizeof(ack_buffer));
    }

    printf("All frames received successfully.\n");
    close(client_sock);  // Close socket
    return 0;
}
