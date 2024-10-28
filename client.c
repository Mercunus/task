#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5555
#define BUFFER_SIZE 1024

void* get_msgs(void* client_socket) {
    char buffer[BUFFER_SIZE];
    while (1) {
        int bytes_received = recv(*(int*)client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Сообщение от сервера: %s\n", buffer);
        }
        else {
            pthread_exit(0);
        }
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    pthread_t id;

    client_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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

    int connection_id = -1;

    if (connection_id < 0) {    
        int bytes_received = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Подключен к серверу с fd: %s\n", buffer);
            connection_id = atoi(buffer);
        }
    }

    pthread_create(&id, NULL, get_msgs, &client_socket);

    printf("Вводите сообщения (введите 'exit' для выхода):\n");

    while (1) {
    
        fgets(buffer, BUFFER_SIZE, stdin);
        if (strncmp(buffer, "exit", 4) == 0) {
            break;
        }
        send(client_socket, buffer, strlen(buffer), 0);
    }

    close(client_socket);
    pthread_cancel(id);
    pthread_join(id, NULL);
    return 0;
}
