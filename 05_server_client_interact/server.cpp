#include "common.h"

void read_data(int sockfd) {
    ssize_t n;
    char buf[1024];

    int time = 0;
    while (true) {
        fprintf(stdout, "block in read\n");
        // 0 means EOF
        if ((n == readn(sockfd, buf, 1024)) == 0) {
            return;
        }

        time++;
        fprintf(stdout, "1K read for %d \n", time);
        usleep(1000);
    }
}

int main() {
    int listen_fd;
    int conn_fd;
    struct sockaddr_in client_addr, server_addr;

    listen_fd = socket(PF_INET, SOCK_STREAM, 0);

    // write zeroes to a byte string
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET; /* Address family     */
    server_addr.sin_port = SERVER_PORT; /* Port number            */
    // htonl(): convert 32 bit quantities between network byte order and host byte order.  (Network byte order is big endian, or most significant byte first.)  On machines which have a byte order which is the same as the network order, routines are defined as null macros.
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // When a socket is created with socket(2) it exists in a name space (address family) but has no name assigned.  bind() requests that address be assigned to the socket.
    bind(listen_fd, (sockaddr *) &server_addr, sizeof(server_addr));

    listen(listen_fd, MAX_LEN_QUEUE_PENDING_CONN);

    while(true) {
        socklen_t client_len;
        client_len = sizeof(client_addr);
        // The first argument socket is a socket that has been created with socket(2), bound to an address with bind(2), and is listening for connections after a listen(2).  accept() extracts the first connection request on the queue of pending connections, creates a new socket with the same properties of socket, and allocates a new file descriptor for the socket.  If no pending connections are present on the queue, and the socket is not marked as non-blocking, accept() blocks the caller until a connection is present.  If the socket is marked non-blocking and no pending connections are present on the queue, accept() returns an error as described below.  The accepted socket may not be used to accept more connections.  The original socket socket, remains open.
        conn_fd = accept(listen_fd, (sockaddr*) &client_addr, &client_len);
        read_data(conn_fd);
        // close connected socket, not the listening socket
        close(conn_fd);
    }
}
