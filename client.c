#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5555
#define BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        perror("Ошибка при создании сокета");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Ошибка подключения к серверу");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Подключено к серверу. Вводите сообщения (введите 'exit' для выхода):\n");

    while (1) {
    
        fgets(buffer, BUFFER_SIZE, stdin);
        
      
        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }

        send(client_socket, buffer, strlen(buffer), 0);
        
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Сообщение от сервера: %s", buffer);
        }
    }

    close(client_socket);
    return 0;
}
