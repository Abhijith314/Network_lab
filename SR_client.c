// SelectiveR_client_short.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define SERVER_IP "127.0.0.1"
#define PORT 9009
#define TOTAL_FRAMES 9

int is_faulty() {
    return (rand() % 4 > 2); // 25% chance to be faulty
}

int main() {
    srand(time(0));
    int client_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr(SERVER_IP);

    if (connect(client_sock, (struct sockaddr*)&server, sizeof(server)) < 0) {
        perror("Connection failed");
        return 1;
    }

    printf("Client running... Selective Repeat ARQ\n");

    int acked_frames = 0;
    char recv_buffer[100], ack_buffer[50];

    while (acked_frames < TOTAL_FRAMES) {
        bzero(recv_buffer, sizeof(recv_buffer));
        int n = read(client_sock, recv_buffer, sizeof(recv_buffer));
        if (n <= 0) continue;

        int frame_num = recv_buffer[strlen(recv_buffer) - 1] - '0';
        printf("Received: %s\n", recv_buffer);

        if (is_faulty()) {
            snprintf(ack_buffer, sizeof(ack_buffer), "negative ack-%d", frame_num);
            printf("Corrupted frame %d. Sending NACK.\n", frame_num);
        } else {
            snprintf(ack_buffer, sizeof(ack_buffer), "ack-%d", frame_num);
            printf("Frame %d OK. Sending ACK.\n", frame_num);
            acked_frames++;
        }

        write(client_sock, ack_buffer, sizeof(ack_buffer));
    }

    printf("All frames received successfully.\n");
    close(client_sock);
    return 0;
}
