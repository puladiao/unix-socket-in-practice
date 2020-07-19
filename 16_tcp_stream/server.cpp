#include "common.h"

static int count;

static void sig_int(int signo) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

size_t read_message(int fd, char *buffer, size_t length) {
    uint32_t msg_length;
    uint32_t msg_type;

    int rc;
    rc = readn(fd, (char *) &msg_length, sizeof(uint32_t));
    if (rc != sizeof(uint32_t)) {
        printf("parse length failed");
        return rc < 0 ? -1 : 0;
    }
    msg_length = ntohl(msg_length);

    rc = readn(fd, (char *) &msg_type, sizeof(uint32_t));
    if (rc != sizeof(uint32_t)) {
        printf("parse type failed");
        return rc < 0 ? -1 : 0;
    }
    msg_type = ntohl(msg_type);

    if (msg_length > length) {
        printf("msg length[%llu] > length[%llu]", msg_length, length);
        return -1;
    }

    rc = readn(fd, buffer, msg_length);
    if (rc != msg_length){
        printf("parse msg failed");
        return rc < 0 ? -1 : 0;
    }
    return rc;
}

int main() {
    int listen_fd;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int on = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

    int bind_rt = bind(listen_fd, (sockaddr*) &server_addr, sizeof(server_addr));
    if (bind_rt < 0) {
        error(1, errno, "bind failed");
    }

    // set socket as passive
    int listen_rt = listen(listen_fd, MAX_LEN_QUEUE_PENDING_CONN);
    if (listen_rt < 0) {
        error(1, errno, "listen failed");
    }

    signal(SIGPIPE, sig_int);

    int conn_fd;
    sockaddr_in client_addr;
    socklen_t client_len;

    conn_fd = accept(listen_fd, (sockaddr*)&client_addr, &client_len);
    if (conn_fd < 0) {
        error(1, errno, "listen failed");
    }

    char buf[128];
    count = 0;
    while (true) {
        int n = read_message(conn_fd, buf, sizeof(buf));
        if (n < 0) {
            error(1, errno, "error read message");
        } else if (n == 0) {
            error(1, 0, "client closed \n");
        }

        buf[n] = 0;
        printf("received %d bytes: %s\n", n, buf);
        count++;
    }

    exit(0);
}
