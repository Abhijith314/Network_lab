// SelectiveR_server_short.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

// Configuration constants
#define PORT 9009             // Port number for communication
#define TOTAL_FRAMES 5       // Total number of frames to send
#define WINDOW_SIZE 2        // Size of the sliding window

/* Function to resend a specific frame */
void resend_frame(int frame, int client_sock) {
    char buffer[60];
    // Format the resend message
    snprintf(buffer, sizeof(buffer), "server message :%d", frame);
    printf("Resending Message to client : %s\n", buffer);
    // Send the frame to client
    write(client_sock, buffer, sizeof(buffer));
    usleep(1000); // Small delay to prevent flooding
}

int main() {
    // Create TCP socket
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        return 1;
    }

    // Configure server address
    struct sockaddr_in server = {0}, client = {0};
    server.sin_family = AF_INET;           // IPv4
    server.sin_port = htons(PORT);         // Port in network byte order
    server.sin_addr.s_addr = INADDR_ANY;   // Accept connections on any interface

    // Bind socket to the server address
    if (bind(server_sock, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("Bind failed");
        return 1;
    }

    // Listen for incoming connections (backlog queue of 5)
    listen(server_sock, 5);
    printf("Server is running... Selective Repeat ARQ\n");

    // Accept client connection
    socklen_t len = sizeof(client);
    int client_sock = accept(server_sock, (struct sockaddr*)&client, &len);
    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }

    int total_acked = 0;       // Counter for acknowledged frames
    char recv_buffer[50];      // Buffer for receiving ACKs

    // Main transmission loop
    while (total_acked < TOTAL_FRAMES) {
        int base = total_acked; // Base of current window

        // Send all frames in current window
        for (int i = base; i < base + WINDOW_SIZE && i < TOTAL_FRAMES; i++) {
            char send_buffer[60];
            // Format the frame message
            snprintf(send_buffer, sizeof(send_buffer), "server message :%d", i);
            printf("Sending: %s\n", send_buffer);
            // Send the frame
            write(client_sock, send_buffer, sizeof(send_buffer));
            usleep(1000); // Small delay between sends
        }

        // Wait for ACKs for each frame in window
        for (int i = base; i < base + WINDOW_SIZE && i < TOTAL_FRAMES; i++) {
            fd_set readfds;             // File descriptor set for select()
            struct timeval timeout = {2, 0}; // 2 second timeout
            
            // Initialize select() parameters
            FD_ZERO(&readfds);
            FD_SET(client_sock, &readfds);

            // Wait for ACK with timeout
            int rv = select(client_sock + 1, &readfds, NULL, NULL, &timeout);
            
            if (rv == 0) {
                // Timeout occurred - frame not acknowledged
                printf("Timeout! Frame %d not acknowledged.\n", i);
                resend_frame(i, client_sock); // Resend only this frame
                i--; // Retry same frame
            } 
            else if (rv > 0) {
                // Received response from client
                read(client_sock, recv_buffer, sizeof(recv_buffer));
                printf("Received from client: %s\n", recv_buffer);

                if (recv_buffer[0] == 'n') {
                    // Negative ACK received (frame corrupted)
                    int corrupt_frame = recv_buffer[strlen(recv_buffer) - 1] - '0';
                    printf("Corrupt ACK for frame %d. Resending...\n", corrupt_frame);
                    resend_frame(corrupt_frame, client_sock); // Resend only corrupt frame
                    i--; // Retry same frame
                } else {
                    // Positive ACK received
                    total_acked++; // Move window forward
                }
            } 
            else {
                // Error in select()
                perror("Select error");
            }
        }
    }

    printf("All frames sent and acknowledged successfully.\n");
    close(client_sock);
    close(server_sock);
    return 0;
}
