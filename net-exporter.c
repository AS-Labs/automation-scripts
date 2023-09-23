#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <sys/types.h>

#define BUFFER_SIZE 65536

void parseConnections(char* buffer, size_t length, struct in_addr* currentAddr) {
    struct ip* ipHeader = (struct ip*)buffer;

    if (ipHeader->ip_p == IPPROTO_TCP) {
        struct tcphdr* tcpHeader = (struct tcphdr*)(buffer + ipHeader->ip_hl * 4);
        struct in_addr sourceAddr, destAddr;

        sourceAddr.s_addr = ipHeader->ip_src.s_addr;
        destAddr.s_addr = ipHeader->ip_dst.s_addr;

        if (destAddr.s_addr == currentAddr->s_addr) {
            printf("[%s, %s, 1]\n", inet_ntoa(sourceAddr), inet_ntoa(*currentAddr));
        }
    }
}

int main() {
    char buffer[BUFFER_SIZE];
    struct in_addr currentAddr;

    // Replace with the IP address of the current server
    if (inet_pton(AF_INET, "127.0.0.1", &(currentAddr.s_addr)) <= 0) {
        perror("Invalid address");
        return 1;
    }

    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_TCP);
    if (sock < 0) {
        perror("Socket creation error");
        return 1;
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytesRead = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesRead < 0) {
            perror("Receive error");
            close(sock);
            return 1;
        }

        parseConnections(buffer, bytesRead, &currentAddr);
    }

    close(sock);
    return 0;
}

