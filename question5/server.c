#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>

#define MAX_CLIENTS 5
#define PORT 8080

// Structured Message Protocol
typedef enum { AUTH, VIEW, RESERVE, EXIT, SUCCESS, FAILURE } MsgType;

typedef struct {
    MsgType type;
    char content[256];
    int book_id;
} Payload;

// Shared Resources
char *books[] = {"Introduction to C", "Distributed Systems 101", "TCP/IP Illustrated"};
int reserved[3] = {0, 0, 0}; // 0 = Available, 1 = Reserved
int active_users = 0;
pthread_mutex_t library_mutex = PTHREAD_MUTEX_INITIALIZER;

void* handle_client(void* socket_desc) {
    int sock = *(int*)socket_desc;
    Payload msg;
    char user_id[50] = "Unknown";
    int authenticated = 0;

    while (recv(sock, &msg, sizeof(Payload), 0) > 0) {
        pthread_mutex_lock(&library_mutex);

        if (msg.type == AUTH) {
            // Simple logic: IDs starting with 'LIB' are valid
            if (strncmp(msg.content, "LIB", 3) == 0) {
                authenticated = 1;
                strcpy(user_id, msg.content);
                active_users++;
                msg.type = SUCCESS;
                printf("[SERVER] User %s authenticated. Active users: %d\n", user_id, active_users);
            } else {
                msg.type = FAILURE;
            }
        } 
        else if (authenticated && msg.type == VIEW) {
            msg.type = SUCCESS;
            snprintf(msg.content, 256, "1. %s [%s]\n2. %s [%s]\n3. %s [%s]", 
                books[0], reserved[0] ? "Reserved" : "Available",
                books[1], reserved[1] ? "Reserved" : "Available",
                books[2], reserved[2] ? "Reserved" : "Available");
        }
        else if (authenticated && msg.type == RESERVE) {
            int id = msg.book_id - 1;
            if (id >= 0 && id < 3 && !reserved[id]) {
                reserved[id] = 1;
                msg.type = SUCCESS;
                printf("[SERVER] %s reserved '%s'\n", user_id, books[id]);
            } else {
                msg.type = FAILURE;
            }
        }

        pthread_mutex_unlock(&library_mutex);
        send(sock, &msg, sizeof(Payload), 0);
        if (msg.type == EXIT) break;
    }

    close(sock);
    pthread_mutex_lock(&library_mutex);
    if (authenticated) active_users--;
    printf("[SERVER] Client %s disconnected. Active users: %d\n", user_id, active_users);
    pthread_mutex_unlock(&library_mutex);
    free(socket_desc);
    return NULL;
}

int main() {
    int server_fd, client_sock;
    struct sockaddr_in server, client;
    socklen_t c = sizeof(struct sockaddr_in);
    pthread_t tid;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Could not create socket");
        return 1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("Bind failed");
        return 1;
    }

    listen(server_fd, MAX_CLIENTS);
    printf("Digital Library Server started on port %d...\n", PORT);

    // FIX: Declare variables outside and assign inside the loop
    while ((client_sock = accept(server_fd, (struct sockaddr *)&client, &c))) {
        if (client_sock < 0) {
            perror("Accept failed");
            continue;
        }

        int *new_sock = malloc(sizeof(int)); // FIX: Allocate enough space for an int
        *new_sock = client_sock;

        if (pthread_create(&tid, NULL, handle_client, (void*)new_sock) < 0) {
            perror("Could not create thread");
            free(new_sock);
        }

        // Detach the thread so it cleans up itself after finishing
        pthread_detach(tid);
    }

    return 0;
}