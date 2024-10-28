#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>

#define PORT 5555
#define MAX_EVENTS 2
#define BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket, epoll_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    struct epoll_event ev, events[MAX_EVENTS];
    
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_socket < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr =inet_addr("127.0.0.1");
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Socket bind error");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0) {
        perror("Socket listen error");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is up.\n");

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("Error creating epoll");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_socket;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_socket, &ev) == -1) {
        perror("Error adding socket in epoll");
        close(server_socket);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < event_count; i++) {
            if (events[i].data.fd == server_socket) {
             
                client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
                if (client_socket == -1) {
                    perror("Client connection error");
                    continue;
                }

                ev.events = EPOLLIN;
                ev.data.fd = client_socket;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_socket, &ev) == -1) {
                    perror("Error adding client connection in epoll");
                    close(client_socket);
                    continue;
                }
                char buffer[BUFFER_SIZE];
                sprintf(buffer, "%d", client_socket);
                send(client_socket, buffer, strlen(buffer), 0);
                printf("Client connected with.\n");
            } else {
                
                char buffer[BUFFER_SIZE];
                int bytes_received = recv(events[i].data.fd, buffer, sizeof(buffer) - 1, 0);
                
                if (bytes_received <= 0) {
                  
                    printf("Client disconnected .\n");
                    close(events[i].data.fd);
                } else {
                    buffer[bytes_received] = '\0';
                    printf("Message received: %s from fd: %d\n", buffer, events[i].data.fd);
                    int receiver = atoi(strtok(buffer, " "));
                    if (receiver > 0) {
                        char* message = strtok(NULL, "\0");
                        send(receiver, message, strlen(message), 0);
                    }
                }
            }
        }
    }

    close(server_socket);
    close(epoll_fd);
    return 0;
}
