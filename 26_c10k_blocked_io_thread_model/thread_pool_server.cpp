#include "common.h"
#include "loop_echo.h"

typedef struct {
    int number; // maximum fd capacity
    int *fd; // ptr to array
    int front; 
    int rear;
    pthread_mutex_t mutex; // lock
    pthread_cond_t cond; // conditional
} BlockQueue;

// initialize block queue
void block_queue_init(BlockQueue* block_queue, int number) {
    block_queue->number = number;
    block_queue->fd = calloc(number, sizeof(int));
    block_queue->front = block_queue->rear = 0;
    pthread_mutex_init(&block_queue->mutex, NULL);
    pthread_cond_init(&block_queue->cond, NULL);
}

// put a fd to the block queue
void block_queue_push(BlockQueue* block_queue, int fd) {
    pthread_mutex_lock(&block_queue->mutex);
    block_queue->fd[block_queue->rear] = fd;
    if (++block_queue->rear == block_queue->number) {
        block_queue->rear = 0;
    }
    printf("push fd %d", fd);
    // signal waiting thread to fetch fd
    pthread_cond_signal(&block_queue->cond);
    pthread_mutex_unlock(&block_queue->mutex);
}

int block_queue_pop(BlockQueue* block_queue) {
    pthread_mutex_lock(&block_queue->mutex);
    while (block_queue->front == block_queue->rear) {
        pthread_cond_wait(&block_queue->cond, &block_queue->mutex);
    }
    int fd = block_queue->fd[block_queue->front];
    if (++block_queue->front == block_queue->number) {
        block_queue->front = 0;
    }
    printf("pop fd %d", fd);
    pthread_mutex_unlock(&block_queue->mutex);
    return fd;
}

void thread_run(void* arg) {
    pthread_t tid = pthread_self();
    pthread_detach(tid);

    BlockQueue* block_queue = (BlockQueue*) arg;
    while (true) {
        int fd = block_queue_pop(block_queue);
        printf("get fd in thread, fd==%d, tid==%d", fd, tid);
        loop_echo(fd);
    }
}

int main() {
    int listener_fd = tcp_server_listen(SERVER_PORT);

    BlockQueue block_queue;
    block_queue_init(&block_queue, BLOCK_QUEUE_SIZE);

    pthread_t thread_array[THREAD_NUMBER];

    int i;
    for (i = 0; i < THREAD_NUMBER; i++) {
        pthread_create(&thread_array[i], NULL, &thread_run, (void*) &block_queue);
    }

    while (true) {
        struct sockaddr_storage ss;
        socklen_t slen = sizeof(ss);
        int fd = accept(listener_fd, (struct sockaddr*) &ss, &slen);
        if (fd < 0) {
            error(1, errno, "accept failed");
        } else {
            block_queue_push(&block_queue, fd);
        }
    }
}
