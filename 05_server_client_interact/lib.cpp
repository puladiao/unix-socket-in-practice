#include "common.h"
extern int errno;
// read "size" bytes into buffer from socket file descriptor
int readn(int fd, char* buffer, size_t size) {
    char *buffer_pointer = buffer;
    int length = size;

    while (length > 0) {
        int result = read(fd, buffer_pointer, length);

        if (result < 0) {
            // A read from a slow device was interrupted before any data arrived by the delivery of a signal.
            if (errno == EINTR) {
                continue; // no blocking, should read again
            } else {
                return -1; 
            }
        } else if (result == 0) {
            break; // read EOF, because FIN is received
        }
        length -= result;
        // move buffer pointer to the next empty bytes
        buffer_pointer += result;
    }
    return (size - length);
}
