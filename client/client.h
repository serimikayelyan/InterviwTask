#include <netinet/in.h>

#define MESSAGE_LENGTH 1024

// structure to hold information for socket
struct sockaddr_in serv_addr;

// structure to hold ip address of server
struct hostent* server;

// socket file descriptor to connect with server
int sock_fd;

// variable to keep termination signal
int sigterm;

// Handler for gracefully shutdown the server
void sigterm_handler( int signum );

// Create and initialize socket for connections with server
// @params
//  host - ip address of server
//  port - port number to listen
// @return value
//  created socket file descriptor if successfully
int init_server_socket(char* host, int port);

// send command and receive respond to/from server
void communicate();
