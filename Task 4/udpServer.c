// udp_serveur.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serv_addr, client_addr;
    char buffer[BUFFER_SIZE];
    socklen_t addr_len = sizeof(client_addr);
    int count = 0;

    // Création de la socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT);

    // Liaison de la socket à l'adresse
    if (bind(sockfd, (const struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Erreur bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Serveur UDP en écoute sur le port %d...\n", PORT);

    while (1) {
        ssize_t n = recvfrom(sockfd, buffer, BUFFER_SIZE, 0,
                             (struct sockaddr *)&client_addr, &addr_len);
        if (n < 0) {
            perror("Erreur recvfrom");
            break;
        }

        buffer[n] = '\0';
        count++;
        printf("Message %d de %s:%d -> %s\n", count,
               inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
    }

    printf("Total de messages reçus : %d\n", count);
    close(sockfd);
    return 0;
}

