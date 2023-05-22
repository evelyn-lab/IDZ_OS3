#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: %s <ip> <port> <max_clients>\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);
    int maxClients = atoi(argv[3]);
    // Создаем сокет
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Failed to create socket");
        return 1;
    }
    // Адрес сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Failed to bind");
        return 1;
    }
    // Прослушиваем входящие соединения от клиентов
    if (listen(serverSocket, maxClients) < 0) {
        perror("Failed to listen");
        return 1;
    }
    printf("Server started. Listening on %s:%d\n", ip, port);
    while (1) {
        // Подключаем клиента
        struct sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
        if (clientSocket < 0) {
            perror("Failed to accept client connection");
            continue;
        }

        char clientIp[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIp, INET_ADDRSTRLEN);
        printf("Client connected: %s:%d\n", clientIp, ntohs(clientAddr.sin_port));

        char buffer[BUFFER_SIZE];
        while (1) {
            memset(buffer, 0, BUFFER_SIZE);

            // Получаем запрос клиента
            int bytesRead = recv(clientSocket, buffer, BUFFER_SIZE - 1, 0);
            if (bytesRead < 0) {
                perror("Failed to receive data from client");
                break;
            } else if (bytesRead == 0) {
                printf("Client disconnected: %s:%d\n", clientIp, ntohs(clientAddr.sin_port));
                break;
            }

            printf("Received request from client: %s\n", buffer);

            // Обрабатываем запрос клиента
            if (strcmp(buffer, "хочу поесть") == 0) {
                sleep(3);
                strcpy(buffer, "можешь есть");
            } else {
                strcpy(buffer, "подожди, горшок пустой");
            }
          
            // Отправляем ответ клиенту 
            if (send(clientSocket, buffer, strlen(buffer), 0) < 0) {
                perror("Failed to send response to client");
                break;
            }
        }
        // Закрываем сокет клиента 
        close(clientSocket);
    }
    // Закрываем сокет сервера
    close(serverSocket);
    return 0;
}
