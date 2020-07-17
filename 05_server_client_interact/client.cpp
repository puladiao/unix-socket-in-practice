#include "common.h"

#define MESSAGE_SIZE 102400

void send_data(int sockfd) {
    char *query;
    query = (char*)malloc(MESSAGE_SIZE + 1);
    for (int i = 0; i < MESSAGE_SIZE; i++) {
        query[i] = 'a';
    }
    query[MESSAGE_SIZE] = '\0'; // end of char*

    const char *cp;
    cp = query;
    size_t to_be_sent_bytes = strlen(query);
    while (to_be_sent_bytes) {
        int bytes_written = send(sockfd, cp, to_be_sent_bytes, 0);
        fprintf(stdout, "send into buffer %ld \n", bytes_written);
        if (bytes_written <= 0) {
            // error() is a general error-reporting function.  It flushes stdout, and then outputs to stderr the program name, a colon and a space, the message specified by the printf(3)-style format string format, and, if errnum  is  nonzero,  a  second colon  and  a  space followed by the string given by strerror(errnum).
            error(1, errno, "send failed");
            return;
        }
        to_be_sent_bytes -= bytes_written;
        cp += bytes_written;
    }
}

int main() {
    int sockfd;
    sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_port = SERVER_PORT;

    char server_addr_str[INET6_ADDRSTRLEN];
    inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr);

    int connect_rt = connect(sockfd, (sockaddr*) &servaddr, sizeof(servaddr));
    if (connect_rt < 0) {
        error(1, errno, "connect failed");
    }

    send_data(sockfd);
}
