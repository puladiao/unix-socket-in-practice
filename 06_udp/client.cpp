#include "common.h"

#define MAXLINE 4096

int main() {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_DGRAM, 0);

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    // This  function converts the character string src into a network address structure in the 
    // af address family, then copies the network address structure to dst.
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    socklen_t server_len = sizeof(server_addr);

    sockaddr *reply_addr;

    reply_addr = (sockaddr*)malloc(sizeof(server_len));

    char send_line[MAXLINE];
    char recv_line[MAXLINE + 1];
    socklen_t len;

    ssize_t n;

    // char *fgets(char *s, int size, FILE *stream);
    while (fgets(send_line, MAXLINE, stdin) != NULL) {
        int i = strlen(send_line);
        if (send_line[i - 1] == '\n') {
            send_line[i - 1] = 0;
        }

        printf("now sending %s\n", send_line);
        ssize_t rt = sendto(socket_fd, send_line, MAXLINE, 0, (sockaddr*)&server_addr, server_len);

        if (rt < 0) {
            error(1, errno, "send failed");
        }

        printf("sent bytes: %zu \n", rt);

        len = 0;
        n = recvfrom(socket_fd, recv_line, MAXLINE+1, 0, reply_addr, &len);

        recv_line[n] = 0;
        fputs(recv_line, stdout);
        fputs("\n", stdout);
    }
}
