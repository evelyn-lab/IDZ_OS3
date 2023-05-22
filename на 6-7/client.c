#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>

volatile sig_atomic_t keepRunning = 1;

void intHandler(int sig) {
    keepRunning = 0;
}

int main(int argc, char *argv[]) {
    signal(SIGINT, intHandler);
    int client_socket;
    struct sockaddr_in server_addr;
    unsigned short server_port;
    int recv_msg_size;
    char *server_ip;

    if (argc != 3) {
       fprintf(stderr, "Usage: %s <Server IP> <Server Port>\n", argv[0]);
       exit(1);
    }
    server_ip = argv[1];
    server_port = atoi(argv[2]);
    if ((client_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        perror("socket() failed");
        exit(1);
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(server_ip);
    server_addr.sin_port = htons(server_port);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect() failed");
        exit(1);
    }
    printf("Connected to server\n");

    char msg[32];
    int buffer[2];
    buffer[0] = 0;

    while (keepRunning) {
        memset(msg, 0, sizeof(msg));
        recv(client_socket, msg, sizeof(msg), 0);
        printf("[Observer] %s\n", msg);
        send(client_socket, buffer, sizeof(buffer), 0);
    }

    recv(client_socket, msg, sizeof(msg), 0);
    buffer[0] = -1;
    send(client_socket, buffer, sizeof(buffer), 0);

    close(client_socket);
    return 0;
}
