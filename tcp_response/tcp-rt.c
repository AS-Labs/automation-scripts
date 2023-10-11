#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Example: %s SERVERIP PORT\n", argv[0]);
        return 1;
    }

    // first arg is the target servers IP
    const char *server_ip = argv[1];
    // second arg is the target port, converting it from string to int
    int port = atoi(argv[2]);

    // Creating a socket, and sock is where we place the fd from the socket syscall.
    // The arguments of socket(int domain, int type, int protocol)
    // domain = AF_INET i.e IPv4
    // type = SOCK_STREAM i.e TCP socket?
    // protocol = 0 i.e the system will choose this by default but 0 is usually used for TCP
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in server_addr;
}
