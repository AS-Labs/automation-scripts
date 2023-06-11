#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


#define MAX_CONNECTIONS 1000

void establishConnections(const char* serverIP, int serverPort, int numConnecitons) {
    int sockets[MAX_CONNECTIONS];
    struct sockaddr_in serverAddr;


    // Creating the socket
    for (int i = 0; i < numConnections; i++) {
        sockets[i] = socket(AF_INET, SOCK_STREAM, 0);
        if (sockets[i] < 0) {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        }
    }
}
