#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "client.h"

void sigterm_handler( int signum ) {
    sigterm = 1;
}

int init_server_socket(char* host, int port)
{
    // Create socket file
    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        perror("ERROR opening socket");
        return -1;
    }
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        return -1;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    // Set socket to use IPv4
    serv_addr.sin_family = AF_INET;
    // Set server socket address
    memcpy((char*)&serv_addr.sin_addr.s_addr,
            (char *)server->h_addr,
            server->h_length);
    // Set socket listening port
    serv_addr.sin_port = htons(port);
    // Connect to server with specified Host and port in serv_addr
    if (connect(sock_fd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0) {
        perror("ERROR connecting");
        return -1;
    }
    return sock_fd;
}

void communicate()
{
    char buffer[MESSAGE_LENGTH];
    while(1) {
        if (sigterm) {
            break;
        }
        printf("Please enter the command: ");
        memset(buffer, 0, MESSAGE_LENGTH);
        fgets(buffer,MESSAGE_LENGTH,stdin);
        int r = write(sock_fd,buffer,strlen(buffer));
        if (r < 0) {
            perror("ERROR writing to socket");
            break;
        }
        memset(buffer, 0, MESSAGE_LENGTH);
        r = read(sock_fd, buffer, MESSAGE_LENGTH);
        if (r < 0) {
            perror("ERROR reading from socket");
            break;
        }
        printf("%s\n", buffer);
    }
}

int main(int argc, char *argv[])
{
    // subscribe for SIGINT and SIGTERM for gracefully shutdown
    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = sigterm_handler;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);

    if (argc < 3) {
        fprintf(stderr,"usage %s hostname port\n", argv[0]);
        exit(0);
    }
    
    int port = atoi(argv[2]);
    if (0 >= port) {
        fprintf (stderr, "ERROR, wrong port value\n");
        return 1;
    }

    if(init_server_socket(argv[1], port) < 0)
        return 1;

    communicate();
}
