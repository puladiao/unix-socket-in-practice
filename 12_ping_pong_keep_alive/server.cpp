#include "common.h"

int main(int argc, char **argv) {
    if (argc != 2) { 
        error(1, 0, "usage: tcpsever ");  
    }
    int sleepingTime = atoi(argv[1]);

    int listen_fd = 0;
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int rt1 = bind(listen_fd, (sockaddr*) &server_addr, sizeof(server_addr));
    if (rt1 < 0) {
        error(1, errno, "bind failed ");
    }

    int rt2 = listen(listen_fd, MAX_LEN_QUEUE_PENDING_CONN);
    if (rt2 < 0) {
        error(1, errno, "listen failed ");
    }

    int conn_fd;

    sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    if ((conn_fd = accept(listen_fd, (sockaddr*)&client_addr, &client_len)) < 0) {
        error(1, errno, "accept failed ");
    }

    messageObject msg_obj;
    int count = 0;
    
    while (true) {
        int n = read(conn_fd, (char *) &msg_obj, sizeof(msg_obj));
        if (n < 0) {
            error(1, errno, "error read");
        } else if (n == 0) {
            // end of file
            error(1, 0, "client closed \n");
        }

        printf("received %d bytes\n", n);
        count++;

        switch(ntohl(msg_obj.type)) {
            case MSG_TYPE1:
                printf("process MSG_TYPE1 \n");
                break;
            case MSG_TYPE2:
                printf("process MSG_TYPE2 \n");
                break;

            case MSG_PING: {
                messageObject pong_message;
                pong_message.type = MSG_PONG;
                sleep(sleepingTime);

                ssize_t rc = send(conn_fd, (void *)&pong_message, sizeof(pong_message), 0);
                if (rc < 0) {
                    error(1, errno, "send failure");
                }
                break;
            }
            default:
                error(1, 0, "unknown message type (%d)\n", ntohl(msg_obj.type));
        }
    }
}
