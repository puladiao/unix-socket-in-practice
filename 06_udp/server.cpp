#include "common.h"

int count;

static void recvfrom_int(int signo) {
    printf("\nreceived %d datagrams\n", count);
    exit(0);
}

int main() {
    // SOCK_DGRAM: UDP
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("create fd failed");
    }

    sockaddr_in server_addr;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_family = AF_INET;
    // bind on 0.0.0.0
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(fd, (sockaddr*) &server_addr, sizeof(server_addr));

    char message[MAX_LINES];
    count = 0;

    // report datagrams received when SIGINT(CTRL + C) is received
    signal(SIGINT, recvfrom_int);

    sockaddr_in client_addr;
    socklen_t client_len{sizeof(client_addr)};

    while (true) {
        // send back what is received and add "hi"
        ssize_t n = recvfrom(fd, message, MAX_LINES, 0, (sockaddr*)&client_addr, &client_len);
        message[n] = 0;

        printf("recv %d bytse[%s]\n", n, message);

        char send_line[MAX_LINES];
        sprintf(send_line, "Hi, %s", message);

        sendto(fd, send_line, strlen(send_line), 0, (sockaddr*)&client_addr, client_len);
        count++;
    }
}
