#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

// Buffer for sending file data
char send_buf[10000];

// Function to read a file and store its content in the send_buf
void send_file(FILE *fp) {
    char ch;
    int i = 0;

    // Read the file character by character and store it in the buffer
    while ((ch = fgetc(fp)) != EOF) {
        send_buf[i++] = ch;
    }
    // Null-terminate the buffer
    send_buf[i] = '\0';
}

int main(int argc, char *argv[]) {
    // Check for proper command-line argument
    if (argc < 2) {
        printf("Usage: ./server <control port>\n");
        exit(1);
    }

    // Define structures for client and server addresses
    struct sockaddr_in client, control, data, data_client;
    FILE *fp;
    int lfd, confd, fd, confd2;
    char rBuf[100] = "";
    int port = atoi(argv[1]);

    // Create a socket for control communication
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd < 0) {
        perror("Error creating socket");
        exit(1);
    }

    // Set up the control socket address structure
    control.sin_family = AF_INET;
    control.sin_port = htons(port);
    control.sin_addr.s_addr = INADDR_ANY;

    // Bind the control socket to the specified port
    if (bind(lfd, (struct sockaddr *)&control, sizeof(control)) < 0) {
        perror("Bind failed");
        close(lfd);
        exit(1);
    }

    // Listen for incoming connections
    listen(lfd, 5);
    printf("\nServer ready, waiting for client....\n");

    // Accept a connection from a client
    socklen_t n = sizeof(client);
    confd = accept(lfd, (struct sockaddr *)&client, &n);
    if (confd < 0) {
        perror("Accept failed");
        close(lfd);
        exit(1);
    }

    printf("\nClient connected....\n");

    while (1) {
        // Receive a command from the client
        recv(confd, rBuf, sizeof(rBuf), 0);

        // Check if the command is "get"
        if (strcmp(rBuf, "get") == 0) {
            printf("Client command: %s\n", rBuf);

            // Receive the filename from the client
            recv(confd, rBuf, sizeof(rBuf), 0);
            printf("\nServer received the file to fetch: %s\n", rBuf);

            // Open the requested file
            fp = fopen(rBuf, "r");
            if (fp == NULL) {
                // If file cannot be opened, send an error message to the client
                send(confd, "error", sizeof("error"), 0);
                printf("\nError in opening file: %s\n", rBuf);
            } else {
                // Send a success message to the client
                send(confd, "success", sizeof("success"), 0);

                // Create a socket for data transfer
                fd = socket(AF_INET, SOCK_STREAM, 0);
                if (fd < 0) {
                    perror("Error creating data socket");
                    exit(1);
                }

                // Set up the data socket address structure
                data.sin_family = AF_INET;
                data.sin_addr.s_addr = INADDR_ANY;
                data.sin_port = htons(8080);

                // Bind the data socket to the specified port
                if (bind(fd, (struct sockaddr *)&data, sizeof(data)) < 0) {
                    perror("Bind failed for data socket");
                    close(fd);
                    exit(1);
                }

                // Listen for incoming data connections
                listen(fd, 5);
                printf("\nServer ready, waiting for client to connect to data socket\n");

                // Accept a connection from the client for data transfer
                n = sizeof(data_client);
                confd2 = accept(fd, (struct sockaddr *)&data_client, &n);
                if (confd2 < 0) {
                    perror("Accept failed for data socket");
                    close(fd);
                    exit(1);
                }

                printf("\nClient connected to data socket\n");

                // Read the file content into the buffer
                send_file(fp);

                // Send the file content to the client
                if (send(confd2, send_buf, sizeof(send_buf), 0) < 0) {
                    printf("Sending failed\n");
                } else {
                    printf("File sent successfully\n");
                }

                // Close the file and the data socket
                fclose(fp);
                close(confd2);
                close(fd);
            }
        } else if (strcmp(rBuf, "close") == 0) {
            // If the command is "close", break the loop and close the connection
            printf("\nClient command: %s\n", rBuf);
            printf("\nClient disconnected and server closed\n");
            break;
        }
    }

    // Close the control socket and exit
    close(confd);
    close(lfd);
    return 0;
}
