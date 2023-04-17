#ifndef INTERRUPT_TRANSFER_QUEUE_H
#define INTERRUPT_TRANSFER_QUEUE_H

#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include "transfer.h"

typedef struct {
    interrupt_transfer_t *buffer;
    int capacity;
    int front;
    int rear;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_empty;
    pthread_cond_t not_full;
} interrupt_transfer_queue_t;

typedef struct interrupt_transfer_queue_t interrupt_transfer_queue_t;

interrupt_transfer_queue_t* interrupt_transfer_queue_create(int endpoint_fd, bool *shutdown_flag);
void interrupt_transfer_queue_init(interrupt_transfer_queue_t *queue, int capacity);
void interrupt_transfer_queue_destroy(interrupt_transfer_queue_t *queue);
void interrupt_transfer_queue_enqueue(interrupt_transfer_queue_t *queue, interrupt_transfer_t transfer);
void interrupt_transfer_queue_dequeue(interrupt_transfer_queue_t *queue, interrupt_transfer_t *transfer);

#endif /* INTERRUPT_TRANSFER_QUEUE_H */
