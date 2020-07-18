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

#define SERVER_PORT 12345
// maximum length for the queue of pending connections
#define MAX_LEN_QUEUE_PENDING_CONN 1000
#define MAX_LINES 1000

int readn(int fd, char* buffer, size_t size);
