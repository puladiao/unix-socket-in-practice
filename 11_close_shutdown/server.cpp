#include "common.h"

int count = 0;

static void sig_int(int signo) {    
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

int main() {
    int listen_fd;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    int bind_ret = bind(listen_fd, (sockaddr*)&server_addr, sizeof(server_addr));
    if (bind_ret == -1) {
        error(1, errno, "bind error");
    }

    int listen_ret = listen(listen_fd, 1024);
    if (listen_ret == -1) {
        error(1, errno, "bind error");
    }

    char text_read[MAX_LINES];
    char text_write[MAX_LINES];

    sockaddr_in client_addr;
    socklen_t client_len;

    signal(SIGINT, sig_int);    
    signal(SIGPIPE, SIG_IGN);

    int conn_fd = accept(listen_fd, (sockaddr *)&client_addr, &client_len);
    while(true) {
        ssize_t bytes_read = read(conn_fd, text_read, MAX_LINES);

        if (bytes_read > 0) {
            text_read[bytes_read] = 0;
        } else if (bytes_read == 0) {
            error(1, 0, "client closed \n");
        } else {
            error(1, errno, "error read\n");
        }
        printf("received %d bytes: %s\n", bytes_read, text_read);
        count++;

        sleep(2);
        sprintf(text_write, "Hi, %s", text_read);

        ssize_t bytes_write = write(conn_fd, text_write, strlen(text_write));
        if (bytes_write < 0) {            
            error(1, errno, "error write");         
        }
    }
} 
