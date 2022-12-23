#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

#include "server.h"

// mutex to lock access to the shared array of clients info.
pthread_mutex_t clients_array_lock;

// variable to keep termination signal
int sigterm = 0;

// main socket file descriptor
int sock_fd;

void sigterm_handler( int signum )
{
    fflush(stdout);
    fflush(stderr);
    sigterm = 1;
}

void execute_command(char* command, char* output)
{
    FILE* f;
    char buf[MESSAGE_LENGTH];
    if (strlen(command) > MESSAGE_LENGTH - 6) {
        memcpy(output, "command is longer then 1024", 27);
        return;
    }
    memcpy(command + strlen(command) - 1, " 2>&1", 5);
    f = popen(command, "r");
    int count = 0, size = 0;
    char c;
    while (1) {
        c = fgetc(f);
        if (feof(f))
            break;
        memcpy(output + count, &c, 1);
        count++;
        if (count > MESSAGE_LENGTH)
            break;
    }
    if (0 >= count)
        memcpy(output, "no output\n", 10);
}

void* communicate_with_client(void *arg)
{
    struct client_info* client = (struct client_info*)arg;
    char bufferin[MESSAGE_LENGTH];
    char bufferout[MESSAGE_LENGTH];
    memcpy(bufferout, "connection succedeed", 20);
    int r = write(client->fd, bufferout, strlen(bufferout));
    while (1) {
        memset(bufferin, 0, MESSAGE_LENGTH);
        r = read(client->fd, bufferin, MESSAGE_LENGTH);
        if (r < 0) {
            perror("ERROR reading from socket\n");
            fflush(stderr);
            break;
        }
        memset(bufferout, 0, MESSAGE_LENGTH);
        if (0 == strcmp(bufferin, "disconnect\n")) {
            memcpy(bufferout, "disconnection done", 18);
            r = write(client->fd, bufferout, strlen(bufferout));
            printf("client disconnected: %p\n", client);
            fflush(stdout);
            break;
        } else {
            printf("client: %p, shell command: %s\n", client, bufferin);
            fflush(stdout);
            execute_command(bufferin, bufferout);
        }
        r = write(client->fd, bufferout, strlen(bufferout));
        if (r < 0) {
            perror("ERROR writing to socket\n");
            fflush(stderr);
            break;
        }
    }
    remove_client(client);
}

void add_client(struct client_info* client)
{
    pthread_mutex_lock(&clients_array_lock);
    int msize = (client_size + 1) * sizeof(struct client_info*);
    struct client_info** new_clients = malloc(msize);
    memcpy(new_clients, clients, client_size * sizeof(struct client_info*));
    free(clients);
    new_clients[client_size] = client;
    clients = new_clients;
    client_size++;
    pthread_mutex_unlock(&clients_array_lock);
}

void remove_client(struct client_info* client)
{
    pthread_mutex_lock(&clients_array_lock);
    int msize = (client_size - 1) * sizeof(struct client_info*);
    struct client_info** new_clients = malloc(msize);
    for (int i = 0,j = 0; i < client_size; i++) {
        if (client != clients[i]) {
            new_clients[j] = clients[i];
            j++;
        }
    }
    close(client->fd);
    free(client);
    free(clients);
    clients = new_clients;
    client_size--;
    pthread_mutex_unlock(&clients_array_lock);
}

int init_server_socket(int port)
{
    struct sockaddr_in serv_addr;
    // Create socket file
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("ERROR opening socket\n");
        fflush(stderr);
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
        perror ("ERROR on binding\n");
        fflush(stderr);
        return -1;
    }
    listen(sock_fd, 5);
    pthread_mutex_init(&clients_array_lock, NULL);
    return sock_fd;
}

void accept_clients()
{
    while (1) {
        struct client_info* c = malloc(sizeof(struct client_info));
        struct sockaddr_in cli_addr;
        int clilen = sizeof(c->socket_addr);
        sleep(1);
        if (sigterm) {
            printf("terminated\n");
            fflush(stdout);
            break;
        }
        c->fd = accept(sock_fd, (struct sockaddr *) &(c->socket_addr), &clilen);
        if (c->fd < 0) {
            perror("ERROR on accept\n");
            fflush(stderr);
            free(c);
            continue;
        }
        add_client(c);
        if (MAX_CLIENT_COUNT < client_size) {
            int r = write(c->fd, "max conenction excided\n", 23);
            if (r < 0)
                perror("ERROR writing to socket\n");
                fflush(stderr);
            remove_client(c);
            printf("client not accepted: %p\n", c);
            fflush(stdout);
            continue;
        }
        pthread_create(&c->thread, NULL, communicate_with_client, c);
        printf("new client connected: %p\n", c);
        fflush(stdout);
    }
}

void daemonize(char* logfile)
{
    printf("PID: %d\n", getpid());
    freopen(logfile,"a",stdout);
    freopen(logfile,"a",stderr);
    daemon(1, 1);
}

int main(int argc, char *argv[])
{

    // subscribe for SIGINT and SIGTERM for gracefully shutdown
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = sigterm_handler;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

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
    daemonize("/tmp/server.log");
    accept_clients();
    printf("application finished\n");
}
