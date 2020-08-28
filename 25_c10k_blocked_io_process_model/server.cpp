#include "common.h"

char rot13_char(char c) {
    if ((c >= 'a' && c <= 'm') || (c >= 'A' && c <= 'M'))
        return c + 13;
    else if ((c >= 'n' && c <= 'z') || (c >= 'N' && c <= 'Z'))
        return c - 13;
    else
        return c;
}

void child_run(int fd) {
    char response[MAX_LINES + 1];
    size_t response_used = 0;
    ssize_t result;

    while (true) {
        char ch;
        // read one char
        result = recv(fd, &ch, 1, 0);
        if (result == 0) {
            // peer performed an orderly shutdown
            break;
        } else if (result  == -1) {
            perror("recv error");
            break;
        } 

        // no error, prepare response
        if (response_used < sizeof(response)) {
            response[response_used++] = rot13_char(ch);
        }

        // for each new line, send a response
        if (ch == '\n') {
            send(fd, response, response_used, 0);
            response_used = 0;
            continue;
        }
    }
}

void sigchld_handler(int sig) {
    // pid : -1 wait for any child process.
    // status: 0 NULL, do not store status
    // WNOHANG: return immediately if no child has exited
    // wait for any child process to exit and clean zombie process
    while (waitpid(-1, 0, WNOHANG) > 0);
    return;
}

int main() {
    int listener_fd = tcp_server_listen(SERVER_PORT);
    signal(SIGCHLD, sigchld_handler);

    while (true) {
        struct sockaddr_storage ss;
        socklen_t slen = sizeof(ss);
        int fd = accept(listener_fd, (struct sockaddr *)&ss, &slen);
        if (fd < 0) {
            error(1, errno, "accept failed");
            exit(1);
        }

        if (fork() == 0) { // child process
            close(listener_fd);
            child_run(fd);
            printf("child process exit");
            exit(0);
        } else {
            close(fd);
        }
    }
    return 0;
}
