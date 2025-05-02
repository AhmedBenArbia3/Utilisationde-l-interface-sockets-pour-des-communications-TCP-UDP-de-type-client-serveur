// serveur_concurrent.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void handle_client(int client_sock, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    int count = 0;

    while (1) {
        ssize_t n = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
        if (n <= 0) break;

        buffer[n] = '\0';
        count++;
        printf("[PID %d] Reçu de %s:%d (%d) -> %s\n",
               getpid(), inet_ntoa(client_addr.sin_addr),
               ntohs(client_addr.sin_port), count, buffer);

        sleep(1);  // Simule un traitement long
    }

    printf("[PID %d] Fin de communication avec le client.\n", getpid());
    close(client_sock);
    exit(0);  // Termine le processus fils
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    signal(SIGCHLD, SIG_IGN); // Évite les zombies

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Erreur socket");
        exit(1);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur bind");
        exit(1);
    }

    listen(server_sock, 5);
    printf("Serveur TCP concurrent en écoute sur le port %d\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Erreur accept");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(server_sock);  // Fermeture du socket parent dans le fils
            handle_client(client_sock, client_addr);
        } else if (pid > 0) {
            close(client_sock);  // Fermeture du socket client dans le parent
        } else {
            perror("Erreur fork");
        }
    }

    close(server_sock);
    return 0;
}

