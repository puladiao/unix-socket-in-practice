#include "common.h"

int main() {
    int listen_fd;
    int connected_fd;
    int ready_number;
    ssize_t n;
    char buf[MAX_LINES];
    sockaddr_in client_addr;

    listen_fd = tcp_server_listen(SERVER_PORT);

    pollfd event_set[POLL_SIZE];
    event_set[0].fd = listen_fd;
    event_set[0].events = POLLRDNORM; // There is data to read.

    int i;
    for (i = 1; i < POLL_SIZE; i++) {
        event_set[i].fd = -1; // events field is ignored
    }

    while (true) {
        // Specifying a negative value  in  timeout  means  an  infinite  timeout.
        // Specifying a timeout of zero causes poll() to return immediately, even if no file descriptors are ready.
        printf("wait for poll\n");
        if ((ready_number = poll(event_set, POLL_SIZE, -1)) < 0) {
            error(1, errno, "poll failed");
        }

        // listen_fd has something to read
        if (event_set[0].revents & POLLRDNORM) {
            socklen_t client_len = sizeof(client_addr);
            connected_fd = accept(listen_fd, (sockaddr *) &client_addr, &client_len);
            printf("client accepted\n");

            // put accepted fd to poll set, then in next while loop, if there's data to read
            // poll will set events
            for (i = 1; i < POLL_SIZE; i++) {
                // < 0 means not used yet
                if (event_set[i].fd < 0) {
                    printf("client[%llu] join accepted set\n", connected_fd);
                    event_set[i].fd = connected_fd;
                    event_set[i].events = POLLRDNORM;
                    break;
                }
            }

            // event set full
            if (i == POLL_SIZE) {
                error(i, errno, "cannot hold so many clients");
            }

            printf("ready number %d\n", ready_number);
            // listen fd process complete
            if (--ready_number <= 0) {
                continue;
            }
        }

        for (i = 1; i < POLL_SIZE; i++) {
            printf("loop for poll\n");
            int socket_fd;
            if ((socket_fd = event_set[i].fd) < 0) {
                continue;
            }

            if (event_set[i].revents & (POLLRDNORM | POLLERR)) {
                // read client request and response
                if ((n = read(socket_fd, buf, MAX_LINES)) > 0) {
                    buf[n +1] = 0;
                    printf("client[%llu] msg[%s] received\n", socket_fd, buf);
                    if (write(socket_fd, buf, n) < 0) {
                        error(1, errno, "write error");
                    }
                } else if (n == 0 || errno == ECONNRESET) {
                    close(socket_fd); // client reset
                    event_set[i].fd = -1;
                } else {
                    error(1, errno, "read error");
                }

                if (--ready_number <= 0) {
                    break;
                }
            }
        } 
    }
}
