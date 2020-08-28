#pragma once
// read()
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

// socket
#include <sys/socket.h>
// sockaddr_un
#include <sys/un.h>

#include <arpa/inet.h>
#include <strings.h>

// errno, fgets
#include <stdio.h>
#include <errno.h>

// malloc
#include <stdlib.h>

// strlen
#include <string.h>

// error
#include <error.h>

// signal
#include <signal.h>

// poll
#include <poll.h>

// waitpid
#include <sys/wait.h>

#define SERVER_PORT 12345
// maximum length for the queue of pending connections
#define MAX_LEN_QUEUE_PENDING_CONN 1000
#define MAX_LINES 1000

int readn(int fd, char* buffer, size_t size);

typedef struct { 
    u_int32_t type; 
    char data[1024]; 
} messageObject;

#define MSG_PING 1
#define MSG_PONG 2
#define MSG_TYPE1 11
#define MSG_TYPE2 21

#define KEEP_ALIVE_TIME 10
#define KEEP_ALIVE_INTERVAL 3
#define KEEP_ALIVE_PROBETIMES 3

#define POLL_SIZE 1024

int tcp_server_listen(int port) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    const int on = 1;
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int bind_rt = bind(fd, (sockaddr*)&server_addr, sizeof(server_addr));
    if (bind_rt != 0) {
        error(1, errno, "bind failed");
    }
    // set passive
    int listen_rt = listen(fd, MAX_LEN_QUEUE_PENDING_CONN);
    if (listen_rt != 0) {
        error(1, errno, "listen failed");
    }
    return fd;
}
