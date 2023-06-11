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


}
