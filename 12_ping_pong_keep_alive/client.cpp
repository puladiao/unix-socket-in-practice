#include "common.h"

int main(int argc, char **argv) {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    int connect_rt = connect(socket_fd, (sockaddr*) &server_addr, sizeof(server_addr));

    if (connect_rt < 0) {
        error(1, errno, "connect failed");
    }

    char recv_line[MAX_LINES + 1];

    int n;

    timeval tv;
    int heartbeats = 0;
    tv.tv_sec = KEEP_ALIVE_TIME;
    tv.tv_usec = 0;

    messageObject msg_obj;

    fd_set readmask;
    fd_set allreads;
    FD_ZERO(&allreads);
    FD_SET(socket_fd, &allreads);

    while (true) {
        readmask = allreads;
        int rc = select(socket_fd + 1, &readmask, NULL, NULL, &tv);

        if (rc < 0) {
            error(1, errno, "select failed");
        }

        // keep-alive by sending PING
        if (rc == 0) {
            if (++heartbeats > KEEP_ALIVE_PROBETIMES) {
                error(1, 0, "connection dead\n");
            }
            printf("sending heartbeat #%d\n", heartbeats);
            msg_obj.type = htonl(MSG_PING);
            rc = send(socket_fd, (char *) &msg_obj, sizeof(msg_obj), 0);
            if (rc < 0) {
                error(1, errno, "send failure");
            }
            tv.tv_sec = KEEP_ALIVE_INTERVAL;
            continue;
        }

        // read response
        if (FD_ISSET(socket_fd, &readmask)) {
            n = read(socket_fd, recv_line, MAX_LINES);
            if (n < 0) {
                error(1, errno, "read error");
            } else if (n == 0) {
                // read EOF
                error(1, 0, "server terminated \n");
            }
            printf("received heartbeat, reset heartbeats to 0 \n");
            heartbeats = 0;
            tv.tv_sec = KEEP_ALIVE_TIME;
        }
    }
}
