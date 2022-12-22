#include <netinet/in.h>

// main socket file descriptor
int sock_fd;

// Structure to hold all required client infos
struct client_info
{
    int fd; // socket descriptor for connected client
    struct sockaddr_in socket_addr;
};

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
