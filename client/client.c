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
#include "../common/common.h"

void sigterm_handler( int signum ) {
    sigterm = 1;
}

uint16_t gen_checksum(char* data)
{
    uint16_t length = (uint16_t)strlen(data);
    uint16_t checksum = 0;
    size_t even_length = length - length%2; // Round down to multiple of 2
    int i;
    for (i = 0; i < even_length; i += 2) {
        uint16_t val = data[i] + 256 * data[i+1];
        checksum += val;
    }
    if (i < length) { // Last byte if it's odd length
        checksum += data[i];
    }
    return checksum;
}

int send_message(char* buf, int s_fd)
{
    uint16_t len = (uint16_t)strlen(buf);
    int m_size = sizeof(struct message) + len;
    struct message* mes = malloc(m_size);
    mes->len_l = len & 0xFF;
    mes->len_h = (len >> 8) & 0xFF;
    memcpy(mes->data, buf, len);
    int r = write(s_fd, mes, m_size);
    if (r < 0) {
        perror("ERROR writing to socket\n");
        fflush(stderr);
        return -1;
    }
    return 1;
}

void print_buf(uint8_t* buf, uint16_t size)
{
    for (uint16_t i = 0; i < size; i++)
        printf("%x ", buf[i]);
    printf("\n");
}

int receive_message(struct message** mes, int s_fd)
{
    *mes = malloc(sizeof(struct message));
    uint16_t len = 0, sum = 0;
    int r = read(s_fd, *mes, sizeof(struct message));
    if (r < 0) {
        perror("ERROR reading from socket\n");
        fflush(stderr);
        return -1;
    }
    len = (*mes)->len_l & 0xFF;
    len |= 0xFF00 & ((*mes)->len_h << 8);
    int m_size = sizeof(struct message) + len;
    *mes = malloc(m_size);
    r = read(s_fd, (*mes)->data, len + 1);
    if (r < 0) {
        free(*mes);
        perror("ERROR reading from socket\n");
        fflush(stderr);
        return -1;
    }
    //uint16_t checksum = gen_checksum((*mes)->data);
    //if (checksum != sum) {
    //    free(*mes);
    //    perror("ERROR received message is corrupted\n");
    //    fflush(stderr);
    //    return -1;
    //}
    return 1;
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
    char buffer[MESSAGE_LENGTH];
    struct message* m;
    int r = receive_message(&m, sock_fd); 
    if (r < 0) {
        perror("ERROR reading from socket");
        return -1;
    }
    if (0 == strcmp(m->data, "max conenction excided\n")) {
        close(sock_fd);
        free(m);
        return -1;
    }
    printf("%s\n", m->data);
    free(m);
    return sock_fd;
}

void communicate()
{
    char buffer[MESSAGE_LENGTH];
    struct message* m;
    while(1) {
        if (sigterm) {
            break;
        }
        printf("Please enter the command: ");
        memset(buffer, 0, MESSAGE_LENGTH);
        fgets(buffer,MESSAGE_LENGTH,stdin);
        int r = send_message(buffer, sock_fd);
        if (r < 0) {
            perror("ERROR writing to socket");
            break;
        }
        memset(buffer, 0, MESSAGE_LENGTH);
        r = receive_message(&m, sock_fd); 
        if (r < 0) {
            perror("ERROR reading from socket");
            break;
        }
        printf("%s\n", m->data);
        if (0 == strcmp(buffer, "disconnection done")) {
            free(m);
            return;
        }
        free(m);
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
