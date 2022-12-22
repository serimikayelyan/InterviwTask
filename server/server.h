#include <netinet/in.h>

// variable to keep termination signal
int sigterm = 0;

// main socket file descriptor
int sock_fd;

// Structure to hold all required client infos
struct client_info
{
    int fd; // socket descriptor for connected client
    struct sockaddr_in socket_addr;
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
