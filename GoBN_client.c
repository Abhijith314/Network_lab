#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Network configuration constants
#define SERVER_IP "127.0.0.1"  // Server IP address (localhost)
#define PORT 9060              // Port number to connect to
#define FRAME_SIZE 20          // Maximum size of each frame

/* Function to send an acknowledgment to the server */
void send_ack(int sock, int frame_no) {
    char ack[FRAME_SIZE];
    // Format the ACK message (e.g., "ACK-0")
    snprintf(ack, sizeof(ack), "ACK-%d", frame_no);
    printf("Sending ACK: %s\n", ack);
    // Send the ACK (including null terminator)
    write(sock, ack, strlen(ack) + 1);
}

int main() {
    int client_sock;                   // Client socket descriptor
    struct sockaddr_in server_addr;    // Server address structure

    // Create TCP socket
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;           // IPv4
    server_addr.sin_port = htons(PORT);         // Port in network byte order
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);  // Server IP

    // Connect to server
    if (connect(client_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }
    printf("Connected to server! Ready to receive frames.\n\n");

    char frame[FRAME_SIZE];            // Buffer for received frames
    int expected_frame = 0;            // Next expected frame number

    // Optional: Configuration for simulating packet loss
    int loss_frame_no = 2;             // Frame number to simulate loss for
    int loss_simulated = 0;            // Flag to ensure loss happens only once

    // Main reception loop
    while (1) {
        memset(frame, 0, sizeof(frame));  // Clear frame buffer

        // Receive frame from server
        int bytes_read = read(client_sock, frame, sizeof(frame));
        if (bytes_read <= 0) {
            // Connection closed or error
            break;
        }

        printf("Received: %s\n", frame);

        int frame_no;
        // Extract frame number from received message
        sscanf(frame, "Frame-%d", &frame_no);

        // Check if frame is in order
        if (frame_no == expected_frame) {
            // Simulate ACK loss (for testing Go-Back-N behavior)
            if (frame_no == loss_frame_no && !loss_simulated) {
                printf("Simulating ACK loss for frame-%d! Not sending ACK.\n\n", frame_no);
                loss_simulated = 1;  // Mark that we've simulated loss
                continue;           // Skip sending ACK
            }

            // Send ACK for in-order frame
            send_ack(client_sock, frame_no);
            expected_frame++;  // Move to next expected frame
        } else {
            // Out-of-order frame detected
            printf("Out of order frame! Expected: %d, Got: %d. Discarding.\n\n", 
                  expected_frame, frame_no);
            // Note: In Go-Back-N, client discards out-of-order frames
        }
    }

    printf("All frames received. Closing connection.\n");
    close(client_sock);  // Close socket
    return 0;
}
