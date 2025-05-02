// server.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 12345
#define MAX_CLIENTS 5

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[128];
    int message_count = 0;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY; // Ecoute toutes les interfaces
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, MAX_CLIENTS);

    printf("Serveur en attente de connexion...\n");
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    printf("Connexion acceptée.\n");

    for (int i = 0; i < 60; i++) {
        time_t now = time(NULL);
        snprintf(buffer, sizeof(buffer), "Heure %d: %s", i+1, ctime(&now));
        send(new_socket, buffer, strlen(buffer), 0);
        message_count++;
        sleep(1); // Délai d'une seconde
    }

    printf("Nombre de messages envoyés: %d\n", message_count);
    close(new_socket);
    close(server_fd);
    return 0;
}

