# Requirements

  * g++
  * Make
  * LIBS
    * pthreads
    * c sockets

# Compilation

    git clone <repo> <dir>
    cd <dir>
    make // to build project
    make clean // to clean build directories

# Execution

## Server
    For server execution listening port must be provided as command line argument.
    Executable binary file for server is stored under <dir>/build/server_app.
    Example: $ <dir>build/server_app 1234

## Client
    For client execution server listening port and host(IP Address) must be provided as command line arguments.
    Executable binary file for client is stored under <dir>/build/client_app.
    Example: $ <dir>build/client_app 127.0.0.1 1234
