#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "interrupt_transfer_queue.h"

#define QUEUE_SIZE 10

void interrupt_transfer_queue_init(interrupt_transfer_queue_t *queue, int capacity) {
    queue->buffer = malloc(capacity * sizeof(interrupt_transfer_t));
    if (!queue->buffer) {
        fprintf(stderr, "Failed to allocate memory for interrupt transfer queue\n");
        exit(EXIT_FAILURE);
    }

    queue->capacity = capacity;
    queue->front = 0;
    queue->rear = -1;
    queue->count = 0;

    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
}

interrupt_transfer_queue_t* interrupt_transfer_queue_create(int endpoint_fd, bool *shutdown_flag) {
    (void)endpoint_fd;
    (void)shutdown_flag;

    interrupt_transfer_queue_t *queue = malloc(sizeof(interrupt_transfer_queue_t));
    if (!queue) {
        fprintf(stderr, "Error: Failed to allocate memory for interrupt transfer queue.\n");
        exit(EXIT_FAILURE);
    }

    interrupt_transfer_queue_init(queue, QUEUE_SIZE);

    return queue;
}

void interrupt_transfer_queue_enqueue(interrupt_transfer_queue_t *queue, interrupt_transfer_t transfer) {
    pthread_mutex_lock(&queue->lock);
    while (queue->count == queue->capacity) {
        pthread_cond_wait(&queue->not_full, &queue->lock);
    }
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->buffer[queue->rear] = transfer;
    queue->count++;
    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->lock);
}

void interrupt_transfer_queue_dequeue(interrupt_transfer_queue_t *queue, interrupt_transfer_t *transfer) {
    pthread_mutex_lock(&queue->lock);

    while (queue->count == 0) {
        pthread_cond_wait(&queue->not_empty, &queue->lock);
    }

    *transfer = queue->buffer[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->count--;

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->lock);
}

void interrupt_transfer_queue_destroy(interrupt_transfer_queue_t *queue) {
    free(queue->buffer);
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
}