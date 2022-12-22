#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "server.h"

void add_client(struct client_info* client)
{
    int msize = (client_size + 1) * sizeof(struct client_info*);
    struct client_info** new_clients = malloc(msize);
    memcpy(new_clients, clients, client_size * sizeof(struct client_info*));
    free(clients);
    new_clients[client_size] = client;
    clients = new_clients;
    client_size++;
}

void remove_client(struct client_info* client)
{
    int msize = (client_size - 1) * sizeof(struct client_info*);
    struct client_info** new_clients = malloc(msize);
    for (int i = 0,j = 0; i < client_size; i++) {
        if (client != clients[i]) {
            new_clients[j] = clients[i];
            j++;
        }
    }
    free(clients);
    clients = new_clients;
    client_size--;
}

int init_server_socket(int port)
{
    struct sockaddr_in serv_addr;

    // Create socket file
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("ERROR opening socket");
        return -1;
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    
    // Set socket to use IPv4
    serv_addr.sin_family = AF_INET;

    // Set socket to listen all network interfaces
    serv_addr.sin_addr.s_addr = INADDR_ANY;

    // Set socket listening port
    serv_addr.sin_port = htons(port);

    // bind socket to interface and port set in serv_addr
    if (bind(sock_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror ("ERROR on binding");
        return -1;
    }

    listen(sock_fd, 5);
    return sock_fd;
}

void accept_clients()
{
    while (1) {
        struct client_info* c = malloc(sizeof(struct client_info));
        struct sockaddr_in cli_addr;
        int clilen = sizeof(c->socket_addr);
        c->fd = accept(sock_fd, (struct sockaddr *) &(c->socket_addr), &clilen);
        if (c->fd < 0) {
            perror("ERROR on accept");
            continue;
        }
        add_client(c);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         return 1;
    }
    int port = atoi(argv[1]);
    if (0 >= port) {
        fprintf (stderr, "ERROR, wrong port value\n");
        return 1;
    }
    int sockfd = init_server_socket(port);
    if (sockfd <= 0)
        return sockfd;
    for (int i = 0; i < 50; i++) {
        struct client_info* c = malloc(sizeof(struct client_info));//new client_info;//malloc(sizeof(struct client_info));
        c->fd = i;
        printf("i: %d, c: %p\n", i, c);
        add_client(c);
        for (int i = 0; i < client_size; i++)
            printf("%p ", clients[i]);
        printf("\n");
    }
    printf("5: %p\n", clients[5]);
    remove_client(clients[5]);
    for (int i = 0; i < client_size; i++)
        printf("%p ", clients[i]);
    printf("\nsocket created\n");
    sleep(999);
    printf("socket finish\n");
}
