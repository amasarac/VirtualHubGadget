//isochronous_queue.h

#ifndef ISOCHRONOUS_QUEUE_H
#define ISOCHRONOUS_QUEUE_H

#include <pthread.h>
#include <time.h>

typedef struct isochronous_transfer {
    int endpoint_fd;
    void *data;
    size_t length;
    int rate;
    struct timespec timestamp;
} isochronous_transfer_t;

typedef struct isochronous_transfer_queue {
    isochronous_transfer_t *queue;
    size_t capacity;
    size_t size;
    size_t front;
    size_t rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
} isochronous_transfer_queue_t;

void isochronous_transfer_queue_init(isochronous_transfer_queue_t *q, int capacity);
void isochronous_transfer_queue_destroy(isochronous_transfer_queue_t *q);
int isochronous_transfer_queue_enqueue(isochronous_transfer_queue_t *q, isochronous_transfer_t transfer);
int isochronous_transfer_queue_dequeue(isochronous_transfer_queue_t *q, isochronous_transfer_t *transfer);

#endif // ISOCHRONOUS_QUEUE_H
