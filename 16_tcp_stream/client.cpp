#include "common.h"

int main() {
    int socket_fd;
    socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in server_addr;
    bzero(&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    // The inet_pton() function converts a presentation format address (that is,
    // printable form as held in a character string) to network format (usually
    // a struct in_addr or some other internal binary representation, in network
    // byte order). 
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    socklen_t server_len = sizeof(server_addr);

    int connect_rt = connect(socket_fd, (sockaddr*)&server_addr, server_len);
    if (connect_rt < 0) {
        error(1, errno, "connect failed");
    }

    struct Message{
        uint32_t message_length;
        uint32_t message_type;
        char buf[128];
    };

    Message message;

    int n;

    while (fgets(message.buf, sizeof(message.buf), stdin) != NULL) {
        n = strlen(message.buf);
        message.message_length = htonl(n);
        message.message_type =htonl(1);
        ssize_t send_rt = send(socket_fd, &message, sizeof(message), 0);
        if (send_rt == -1) {
            error(1, errno, "send failure");
        }
        printf("send %d bytes\n", send_rt);
    }
    exit(0);
}
