#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/wait.h>

// Buffer to store received data
char r_Buf[10000];

// Function declaration
void get_function(int csd, struct sockaddr_in data, char *data_addr);

int main(int argc, char *argv[]) {
    // Check if correct number of arguments is provided
    if (argc < 3) {
        printf("Usage: ./client <server IP> <control port> \n");
        exit(1);
    }

    int csd, cport;
    char command[100];
    struct sockaddr_in servaddr, data;

    // Convert control port argument to integer
    cport = atoi(argv[2]);

    // Create a socket
    csd = socket(AF_INET, SOCK_STREAM, 0);
    if (csd < 0) {
        perror("Error creating socket");
        exit(1);
    }
    printf("Socket created\n");

    // Configure server address struct
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port = htons(cport);

    // Connect to the server
    if (connect(csd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Error in connection");
        exit(1);
    }
    printf("Connected to server\n");

    while (1) {
        // Prompt user for command
        printf("Enter 'get' for receiving file from server\nEnter 'close' for closing the connection\n");
        scanf("%s", command);

        if (strcmp(command, "get") == 0) {
            // Send 'get' command to the server
            send(csd, command, sizeof(command), 0);
            get_function(csd, data, argv[1]);
        } else if (strcmp(command, "close") == 0) {
            // Send 'close' command to the server and break the loop
            send(csd, command, sizeof(command), 0);
            break;
        } else {
            printf("Invalid command\n");
        }
    }

    // Close the control socket
    close(csd);
    return 0;
}

void get_function(int csd, struct sockaddr_in data, char *data_addr) {
    char name[256];
    char valid[10];

    // Prompt user for filename
    printf("Enter filename: ");
    scanf("%s", name);
    printf("Name of file=%s\n", name);

    // Send filename to server
    send(csd, name, sizeof(name), 0);

    // Receive validity check from server
    recv(csd, valid, sizeof(valid), 0);
    if (strcmp(valid, "error") == 0) {
        printf("FILE NOT FOUND PROCESS ID = %d\n", getpid());
        return;
    }

    // Create data socket
    int dd = socket(AF_INET, SOCK_STREAM, 0);
    if (dd < 0) {
        perror("Error creating data socket");
        exit(1);
    }

    // Configure data connection address struct
    data.sin_family = AF_INET;
    data.sin_addr.s_addr = inet_addr(data_addr);
    data.sin_port = htons(8080);

    // Connect to the server data port
    if (connect(dd, (struct sockaddr *)&data, sizeof(data)) < 0) {
        perror("Error in connection of data socket");
        close(dd);
        return;
    }

    // Receive file data from server
    if (recv(dd, r_Buf, sizeof(r_Buf), 0) < 0) {
        printf("Failed to receive file\n");
    } else {
        // Open file to write the received data
        FILE *fp = fopen(name, "w");
        if (fp == NULL) {
            perror("Error opening file");
            close(dd);
            return;
        }

        // Write data to file
        fprintf(fp, "%s", r_Buf);
        fprintf(fp, "FILE %s RECEIVED FROM SERVER WITH PROCESS ID = %d\n", name, getpid());
        fclose(fp);
        printf("File received successfully\n");
    }

    // Close the data socket
    close(dd);
}
