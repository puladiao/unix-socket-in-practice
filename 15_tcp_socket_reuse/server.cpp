#include "common.h"

static int count;

static void sig_int(int signo) {
    printf("\nreceived %d requests\n", count);
    exit(0);
}

int main(int argc, char** args) {
    int socket_listen = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (argc == 2 && strcmp(args[1], "reuse") == 0) {
        printf("set reuse mode\n");
        int on = 1;
        setsockopt(socket_listen, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    }

    int bind_rt = bind(socket_listen, (sockaddr*)&server_addr, sizeof(server_addr));
    if (bind_rt < 0) {
        error(1, errno, "bind failed");
    }

    // make socket passive
    int listen_rt = listen(socket_listen, MAX_LEN_QUEUE_PENDING_CONN);
    if (listen_rt < 0) {
        error(1, errno, "listen failed ");
    }

    signal(SIGPIPE, sig_int);

    int conn_fd;
    sockaddr_in client_addr;
    socklen_t client_len;

    if ((conn_fd = accept(socket_listen, (sockaddr*)&client_addr, &client_len)) < 0) {
        error(1, errno, "accept failed");
    }

    char message[MAX_LINES];

    count = 0;

    while (true) {
        ssize_t n = read(conn_fd, message, MAX_LINES);
        if (n < 0) {
            error(1, errno, "error read");
        } else if (n == 0) {
            // EOF = FIN
            error(1, 0, "client closed \n");
        }
        message[n] = 0;
        printf("received %d bytes: %s\n", n, message);
        count++;
    }
}
