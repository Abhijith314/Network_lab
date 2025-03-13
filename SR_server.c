// SelectiveR_server_short.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 9009
#define TOTAL_FRAMES 9
#define WINDOW_SIZE 3

void resend_frame(int frame, int client_sock) {
    char buffer[60];
    snprintf(buffer, sizeof(buffer), "server message :%d", frame);
    printf("Resending Message to client : %s\n", buffer);
    write(client_sock, buffer, sizeof(buffer));
    usleep(1000);
}

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in server = {0}, client = {0};
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_sock, (struct sockaddr*)&server, sizeof(server)) == -1) {
        perror("Bind failed");
        return 1;
    }

    listen(server_sock, 5);
    printf("Server is running... Selective Repeat ARQ\n");

    socklen_t len = sizeof(client);
    int client_sock = accept(server_sock, (struct sockaddr*)&client, &len);
    if (client_sock < 0) {
        perror("Accept failed");
        return 1;
    }

    int total_acked = 0;
    char recv_buffer[50];

    while (total_acked < TOTAL_FRAMES) {
        int base = total_acked;
        for (int i = base; i < base + WINDOW_SIZE && i < TOTAL_FRAMES; i++) {
            char send_buffer[60];
            snprintf(send_buffer, sizeof(send_buffer), "server message :%d", i);
            printf("Sending: %s\n", send_buffer);
            write(client_sock, send_buffer, sizeof(send_buffer));
            usleep(1000);
        }

        for (int i = base; i < base + WINDOW_SIZE && i < TOTAL_FRAMES; i++) {
            fd_set readfds;
            struct timeval timeout = {2, 0};
            FD_ZERO(&readfds);
            FD_SET(client_sock, &readfds);

            int rv = select(client_sock + 1, &readfds, NULL, NULL, &timeout);
            if (rv == 0) {
                printf("Timeout! Frame %d not acknowledged.\n", i);
                resend_frame(i, client_sock);
                i--; // retry same frame
            } else if (rv > 0) {
                read(client_sock, recv_buffer, sizeof(recv_buffer));
                printf("Received from client: %s\n", recv_buffer);

                if (recv_buffer[0] == 'n') {
                    printf("Corrupt ACK for frame %d. Resending...\n", recv_buffer[strlen(recv_buffer) - 1] - '0');
                    resend_frame(recv_buffer[strlen(recv_buffer) - 1] - '0', client_sock);
                    i--; // retry same frame
                } else {
                    total_acked++;
                }
            } else {
                perror("Select error");
            }
        }
    }

    printf("All frames sent and acknowledged successfully.\n");
    close(client_sock);
    close(server_sock);
    return 0;
}
