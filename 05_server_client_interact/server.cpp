#include "common.h"

void read_data(int accepted_sock_fd) {
    int ret;
    char buf[1024];

    int time = 0;
    while (true) {
        printf("block in read\n");
        // 0 means EOF
        ret = readn(accepted_sock_fd, buf, 1024);
        if (ret == 0) {
            return;
        }

        time++;
        printf("1K read for %d bytes[%d]\n", time, ret);
        usleep(1000);
    }
}

int main() {
    int listen_fd;
    int conn_fd;
    struct sockaddr_in client_addr, server_addr;

    // AF_INET: IPv4 Internet protocols
    // SOCK_STREAM: TCP/IP aka. Provides sequenced, reliable, two-way, connection-based byte streams.
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    // write zeroes to a byte string
    bzero(&server_addr, sizeof(server_addr));

    server_addr.sin_family = AF_INET; /* Address family     */
    server_addr.sin_port = SERVER_PORT; /* Port number            */

    // htonl(): convert 32 bit quantities between network byte order and host byte order.  

    // INADDR_ANY is used when you don't need to bind a socket to a specific IP. When you use 
    // this value as the address when calling bind() , the socket accepts connections to all 
    // the IPs of the machine.
    // It's the same as trying to bind to inet_addr("0.0.0.0").
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // bind() requests address be assigned to the socket.
    bind(listen_fd, (sockaddr *) &server_addr, sizeof(server_addr));

    // listen()  marks  the  socket  referred to by sockfd as a **passive socket**
    // The backlog argument defines the maximum length to which the queue of pending connections 
    // for sockfd may grow.
    listen(listen_fd, MAX_LEN_QUEUE_PENDING_CONN);

    while(true) {
        socklen_t client_len;
        client_len = sizeof(client_addr);
        // accept extracts the first connection request on the queue of pending connections for 
        // the listening socket, sock_fd, creates a new connected socket, and returns a new file 
        // descriptor referring to that socket.
        // The newly created socket is not in the listening state.  
        // The original socket sockfd is unaffected by this call.
        // for blocking socket, accept() blocks the caller until a connection is present.
        conn_fd = accept(listen_fd, (sockaddr*) &client_addr, &client_len);
        read_data(conn_fd);
        // close connected socket, not the listening socket
        close(conn_fd);
    }
}
