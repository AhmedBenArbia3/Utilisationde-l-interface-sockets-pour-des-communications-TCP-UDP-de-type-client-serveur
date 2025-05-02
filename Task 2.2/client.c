// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int sock;
    struct sockaddr_in serv_addr;
    char request[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    int valread;

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and connect
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    // Read request from user
    printf("Enter your HTTP request (end with an empty line):\n");

    request[0] = '\0';
    while (1) {
        char line[256];
        fgets(line, sizeof(line), stdin);

        // Strip newline
        size_t len = strlen(line);
        if (line[len - 1] == '\n') line[len - 1] = '\0';

        if (strlen(line) == 0) {
            strcat(request, "\r\n");  // End of headers
            break;
        }

        strcat(request, line);
        strcat(request, "\r\n");
    }

    send(sock, request, strlen(request), 0);

    valread = read(sock, response, BUFFER_SIZE - 1);
    if (valread >= 0) {
        response[valread] = '\0';
        printf("Server response:\n%s\n", response);
    } else {
        perror("Read error");
    }

    close(sock);
    return 0;
}

