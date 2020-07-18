#include "common.h"

int main() {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int bind_ret = bind(socket_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    if (bind_ret == -1) {
        error(1, errno, "bind error");
    }

    int listen_ret = listen(socket_fd, 1024);
    if (listen_ret == -1) {
        error(1, errno, "bind error");
    }

    char text_read[MAX_LINES];
    char text_write[MAX_LINES];

    sockaddr_in client_addr;
    socklen_t client_len;

    while(true) {
        int client_fd = accept(socket_fd, (sockaddr *)&client_addr, &client_len);
        ssize_t bytes_read = read(client_fd, text_read, MAX_LINES);

        if (bytes_read > 0) {
            text_read[bytes_read - 1] = 0;
        }

        sprintf(text_write, "Hi, %s", text_read);

        ssize_t bytes_write = write(client_fd, text_write, strlen(text_write));
    }
} 
