#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

typedef enum { AUTH, VIEW, RESERVE, EXIT, SUCCESS, FAILURE } MsgType;
typedef struct { MsgType type; char content[256]; int book_id; } Payload;

int main() {
    int sock;
    struct sockaddr_in server;
    Payload msg;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(8080);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) return 1;

    printf("Enter Library ID: ");
    scanf("%s", msg.content);
    msg.type = AUTH;
    send(sock, &msg, sizeof(Payload), 0);
    recv(sock, &msg, sizeof(Payload), 0);

    if (msg.type == SUCCESS) {
        printf("Authenticated successfully!\n");
        msg.type = VIEW;
        send(sock, &msg, sizeof(Payload), 0);
        recv(sock, &msg, sizeof(Payload), 0);
        printf("\nAvailable Books:\n%s\n", msg.content);

        printf("\nEnter book number to reserve: ");
        scanf("%d", &msg.book_id);
        msg.type = RESERVE;
        send(sock, &msg, sizeof(Payload), 0);
        recv(sock, &msg, sizeof(Payload), 0);

        if (msg.type == SUCCESS) printf("Reservation Confirmed!\n");
        else printf("Error: Book already reserved or invalid ID.\n");
    } else {
        printf("Authentication Failed.\n");
    }

    close(sock);
    return 0;
}