#include <netinet/in.h>
#include <pthread.h>

#include "../common/common.h"

#pragma once

#define MESSAGE_LENGTH 1024
#define MAX_CLIENT_COUNT 5

// Structure to hold all required client infos
struct client_info
{
    int fd; // socket descriptor for connected client
    struct sockaddr_in socket_addr;
    pthread_t thread;    
};

// Handler for gracefully shutdown the server
void sigterm_handler( int signum );

// array to store connected clients info in dynamic memory
struct client_info** clients = NULL;

// size of conencted clients (size of array "clients")
int client_size = 0;

// dynamically add newly connected client infos
// @params
//  client - pointer to the struct for newly connected client
void add_client(struct client_info* client);

// remove disconnected client infos
// @params
//  client - pointer to the disconnected client info
void remove_client(struct client_info* client);

// Create and initialize socket to listen for connections from clients
// @params
//  port - port number to listen
// @return value
//  created socket file descriptor if successfully
int init_server_socket(int port);

// Accept connection from new clients
void accept_clients();

// function working in thread to handle communication with client
void* communicate_with_client(void *args);

// Send message to the specific client
// @params
//  message - message to send
//  client - specific client to send the message
int send_message(char* message, struct client_info* client);

// Receive message from the specific client
// @params
//  message - buffer to store received message
//  client - specific client to send the message
int receive_message(struct message** mes, struct client_info* client);
