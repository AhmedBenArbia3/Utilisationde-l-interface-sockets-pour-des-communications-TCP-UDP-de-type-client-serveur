// udp_client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 12345
#define BUFFER_SIZE 1024

void get_current_time(char *buffer, size_t size) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    strftime(buffer, size, "Heure: %H:%M:%S", tm_info);
}

int main() {
    int sockfd;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];
    int count = 0;

    // Création de la socket UDP
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erreur socket");
        exit(EXIT_FAILURE);
    }

    // Configuration de l'adresse du serveur
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

    for (int i = 0; i < 60; i++) {
        get_current_time(buffer, sizeof(buffer));
        sendto(sockfd, buffer, strlen(buffer), 0,
               (struct sockaddr *)&serv_addr, sizeof(serv_addr));
        printf("Message %d envoyé: %s\n", ++count, buffer);
        sleep(1);  // Mettre en commentaire pour tester sans délai
    }

    close(sockfd);
    return 0;
}

