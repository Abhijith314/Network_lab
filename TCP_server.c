#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

int main() {
    struct sockaddr_in client, server;
    int sockfd, n, confd, bindsts, lstnsts; 
    char rBuf[100] = "", sBuf[100] = "";
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0); 
    }
    else
        printf("Socket successfully created..\n");
    server.sin_family = AF_INET;
    server.sin_port = 6061; 
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    bindsts = bind(sockfd, (struct sockaddr *)&server, sizeof(server)); 
    if (bindsts != 0) { 
        printf("socket bind failed...\n"); 
        exit(0); 
    } 
    else
        printf("Socket successfully binded..\n"); 
    lstnsts = listen(sockfd, 1); 
    if (lstnsts != 0) { 
        printf("Listen failed...\n"); 
        exit(0); 
    } 
    else
        printf("Server listening..\n"); 
    printf("\nServer ready, waiting for client….\n");
    confd = accept(sockfd, (struct sockaddr *)&client, &n);
    while(1) {
        n = sizeof(client); 
        recv(confd, rBuf, sizeof(rBuf), 0);
        printf("\nClient: %s", rBuf);
        if(strcmp(sBuf, "end\n") == 0 || strcmp(sBuf, "end") == 0) {
            break;
        }
        printf("\nServer: "); 
        fgets(sBuf, sizeof(sBuf), stdin);
        send(confd, sBuf, sizeof(sBuf), 0); 
        if(strcmp(rBuf, "end\n") == 0 || strcmp(rBuf, "end") == 0) {
            break;
        }
        printf("\n"); 
    }
    close(confd); 
    close(sockfd); 
    return 0;
}
