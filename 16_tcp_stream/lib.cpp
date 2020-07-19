#include "common.h"
extern int errno;

// read "size" bytes into buffer from socket file descriptor
int readn(int fd, char* buffer, size_t size) {
    char *buffer_pointer = buffer;
    size_t length = size;

    while (length > 0) {
        int result = read(fd, buffer_pointer, length);

        if (result < 0) {
            // EINTR: A read from a slow device was interrupted before any data arrived by the 
            // delivery of a signal.
            if (errno == EINTR) {
                continue; // no blocking, should read again
            } else {
                return -1; 
            }
        } else if (result == 0) {
            printf("readn end of file\n");
            break; // read EOF, because FIN is received
        }
        length -= result;
        // move buffer pointer to the next empty bytes
        buffer_pointer += result;
    }
    return (size - length);
}
