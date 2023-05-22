#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <ip> <port>\n", argv[0]);
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

    // Создаем сокет
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Failed to create socket");
        return 1;
    }

    // Адрес сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    // Соединение с сервером
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Failed to connect to server");
        return 1;
    }

    printf("Connected to server: %s:%d\n", ip, port);

    while (1) {
        // Вводим данные 
        char request[BUFFER_SIZE];
        printf("Enter your request: ");
        fgets(request, BUFFER_SIZE, stdin);
        request[strcspn(request, "\n")] = 0;  // Remove trailing newline character

        // Отправляем запрос на сервер 
        if (send(clientSocket, request, strlen(request), 0) < 0) {
            perror("Failed to send data to server");
            break;
        }

        // Получаем ответ от сервера
        char response[BUFFER_SIZE];
        memset(response, 0, BUFFER_SIZE);
        if (recv(clientSocket, response, BUFFER_SIZE - 1, 0) < 0) {
            perror("Failed to receive data from server");
            break;
        }

        printf("Server response: %s\n", response);
    }

    // Закрываем сокет
    close(clientSocket);

    return 0;
}
