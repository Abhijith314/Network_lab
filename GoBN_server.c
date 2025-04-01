#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

// Configuration constants
#define PORT 9060                // Port number for communication
#define WINDOW_SIZE 2            // Number of frames that can be sent without waiting for ACK
#define TOTAL_FRAMES 5           // Total number of frames to send
#define FRAME_SIZE 20            // Maximum size of each frame

/* Function to send a frame to the client */
void send_frame(int sock, int frame_no) {
    char frame[FRAME_SIZE];
    // Format the frame with its number (e.g., "Frame-0")
    snprintf(frame, sizeof(frame), "Frame-%d", frame_no);
    printf("Sending: %s\n", frame);
    // Send the frame (including null terminator)
    write(sock, frame, strlen(frame) + 1);
}

/* Function to wait for acknowledgment with timeout */
int wait_for_ack(int sock, int frame_no) {
    char ack[FRAME_SIZE];        // Buffer for acknowledgment
    fd_set read_fds;             // File descriptor set for select()
    struct timeval timeout;      // Timeout structure

    // Initialize the file descriptor set
    FD_ZERO(&read_fds);
    FD_SET(sock, &read_fds);

    // Set timeout to 3 seconds
    timeout.tv_sec = 3;
    timeout.tv_usec = 0;

    // Wait for socket to become readable (ACK received) or timeout
    int retval = select(sock + 1, &read_fds, NULL, NULL, &timeout);

    if (retval == -1) {
        perror("select()");      // Error in select()
        return 0;
    } else if (retval == 0) {
        printf("Timeout! No ACK for frame-%d\n", frame_no);
        return 0;                // Timeout occurred
    } else {
        read(sock, ack, sizeof(ack));  // Read the ACK
        printf("Received ACK: %s\n", ack);
        return 1;                // ACK received successfully
    }
}

int main() {
    int server_sock, client_sock;      // Socket file descriptors
    struct sockaddr_in server_addr, client_addr;  // Address structures
    socklen_t addr_size;               // Size of address structure

    // Create TCP socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;           // IPv4
    server_addr.sin_port = htons(PORT);         // Port number in network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY;   // Accept connections on any interface

    // Bind socket to the server address
    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections (backlog queue of 1)
    listen(server_sock, 1);

    printf("Server is up, waiting for connection...\n");

    // Accept incoming connection
    addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);
    if (client_sock < 0) {
        perror("Accept failed");
        exit(EXIT_FAILURE);
    }

    printf("Client connected! Starting Go-Back-N Protocol (Window Size = %d)\n\n", WINDOW_SIZE);

    int next_frame = 0;  // Next frame number to be sent

    // Main transmission loop
    while (next_frame < TOTAL_FRAMES) {
        int sent_frames = 0;  // Counter for frames sent in this window

        // Send a window of frames (up to WINDOW_SIZE frames)
        for (int i = 0; i < WINDOW_SIZE && next_frame + i < TOTAL_FRAMES; i++) {
            send_frame(client_sock, next_frame + i);
            sent_frames++;
        }

        int acks_received = 0;  // Counter for ACKs received

        // Wait for ACKs for each sent frame
        for (int i = 0; i < sent_frames; i++) {
            int ack = wait_for_ack(client_sock, next_frame + i);
            if (!ack) {
                // If ACK not received, go back to the failed frame
                printf("Go-Back to frame-%d\n\n", next_frame + i);
                next_frame += i;  // Adjust next_frame to retransmit from the failed frame
                break;
            }
            acks_received++;
        }

        // If all ACKs received, move window forward
        if (acks_received == sent_frames) {
            next_frame += sent_frames;
        }
    }

    printf("All frames sent successfully!\n");

    // Cleanup
    close(client_sock);
    close(server_sock);
    return 0;
}
