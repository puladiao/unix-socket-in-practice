#include "common.h"

int main() {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);
    int connect_rt = connect(socket_fd, (sockaddr*)&server_addr, server_len);
    if (connect_rt == -1) {
        error(1, errno, "bind error");
    }


    char send_line[MAX_LINES];
    char recv_line[MAX_LINES + 1];

    ssize_t n;
    fd_set readmask;
    fd_set allreads;

    // FD_ZERO() clears a set.
    FD_ZERO(&allreads);
    // FD_SET() and FD_CLR() respectively add and remove a given file descriptor from a set.
    FD_SET(0, &allreads);
    FD_SET(socket_fd, &allreads);

    while (true) {
        readmask = allreads;
        // first arg: the highest-numbered file descriptor in any of the three sets, plus 1.
        // select() return the number of file descriptors contained in the three returned descriptor sets 
        // (that is, the  total  number  of  bits  that  are  set  readfds, writefds, exceptfds)
        int rc = select(socket_fd + 1, &readmask, NULL, NULL, NULL);
        if (rc <= 0) {
            error(1, errno, "select error");
        }

        if (FD_ISSET(socket_fd, &readmask)) {
            n = read(socket_fd, recv_line, MAX_LINES);
            if (n < 0) {
                error(1, errno, "read error");
            } else if (n == 0) { // zero indicates end of file
                error(1, 0, "server terminated \n");
            }
            recv_line[n] = 0;
            fputs(recv_line, stdout);
            fputs("\n", stdout);
        }

        if (FD_ISSET(0, &readmask)) {
            if (fgets(send_line, MAX_LINES, stdin) != NULL) {
                if (strncmp(send_line, "shutdown", 8) == 0) {
                    FD_CLR(0, &allreads);
                    if (shutdown(socket_fd, SHUT_WR)) {
                        error(1, errno, "shutdown failed");
                    }
                } else if (strncmp(send_line, "close", 5) == 0) {
                    FD_CLR(0, &allreads);
                    if (close(socket_fd)) {
                        error(1, errno, "close failed");
                    }
                    sleep(6);
                    exit(0);
                } else {
                    int i = strlen(send_line);
                    if (send_line[i - 1] == '\n') {
                        send_line[i - 1] = 0;
                    }
                    printf("now sending %s\n", send_line);
                    size_t rt = write(socket_fd, send_line, strlen(send_line));
                    if (rt < 0) {
                        error(1, errno, "write failed");
                    }
                    printf("send bytes %zu \n", rt);
                }
            }
        }
    }
}
