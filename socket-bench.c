#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>


#define MAX_CONNECTIONS 1000

struct ConnectionInfo {
    const char* serverIP;
    int serverPort;
};

void* establishConnections(void* arg) {
    struct ConnectionInfo* connInfo = (struct ConnectionInfo*)arg;

    int socketFd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketFd < 0) {
        perror("Socket creation failed");
        pthread_exit(NULL);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(connInfo->serverPort);
    if (inet_pton(AF_INET, connInfo->serverIP, &(serverAddr.sin_addr)) <= 0) {
        perror("Invalid address/Address not supported");
        pthread_exit(NULL);
    }

    if (connect(socketFd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Connection failed");
        pthread_exit(NULL);
    }

    printf("Connection established successfully!\n");


}
