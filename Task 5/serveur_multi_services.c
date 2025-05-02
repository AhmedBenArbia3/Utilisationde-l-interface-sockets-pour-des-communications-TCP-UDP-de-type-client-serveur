#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <signal.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void handle_client(int client_sock, struct sockaddr_in client_addr) {
    char buffer[BUFFER_SIZE];
    ssize_t n = recv(client_sock, buffer, BUFFER_SIZE - 1, 0);
    if (n <= 0) {
        close(client_sock);
        exit(1);
    }

    buffer[n] = '\0';
    int service_code = atoi(buffer);
    printf("[PID %d] Service demandé : %d\n", getpid(), service_code);

    switch (service_code) {
        case 1: // Service heure
            for (int i = 0; i < 10; i++) {
                time_t now = time(NULL);
                snprintf(buffer, sizeof(buffer), "Heure: %s", ctime(&now));
                send(client_sock, buffer, strlen(buffer), 0);
                sleep(1);
            }
            break;

        case 2: // Commande distante
            {
                FILE *fp = popen("ps aux | wc -l", "r");
                if (fp) {
                    fgets(buffer, sizeof(buffer), fp);
                    send(client_sock, buffer, strlen(buffer), 0);
                    pclose(fp);
                }
            }
            break;

        case 3: // Transfert fichier
            {
                FILE *file = fopen("example.txt", "r");
                if (file) {
                    while (fgets(buffer, sizeof(buffer), file)) {
                        send(client_sock, buffer, strlen(buffer), 0);
                    }
                    fclose(file);
                } else {
                    snprintf(buffer, sizeof(buffer), "Erreur d'ouverture de fichier.\n");
                    send(client_sock, buffer, strlen(buffer), 0);
                }
            }
            break;

        default:
            snprintf(buffer, sizeof(buffer), "Service inconnu.\n");
            send(client_sock, buffer, strlen(buffer), 0);
            break;
    }

    close(client_sock);
    exit(0);
}

int main() {
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    signal(SIGCHLD, SIG_IGN); // éviter les processus zombies

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Erreur de création de socket");
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Erreur de bind");
        close(server_sock);
        exit(1);
    }

    if (listen(server_sock, 5) < 0) {
        perror("Erreur d'écoute");
        close(server_sock);
        exit(1);
    }

    printf("Serveur multi-services en écoute sur le port %d...\n", PORT);

    while (1) {
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Erreur accept");
            continue;
        }

        pid_t pid = fork();
        if (pid == 0) {
            close(server_sock); // le fils n'a pas besoin du socket d'écoute
            handle_client(client_sock, client_addr);
        } else if (pid > 0) {
            close(client_sock); // le père n'a pas besoin du socket de connexion
        } else {
            perror("Erreur fork");
        }
    }

    close(server_sock);
    return 0;
}

