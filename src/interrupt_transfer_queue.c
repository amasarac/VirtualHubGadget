#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include "interrupt_transfer_queue.h"
#include "common.h"

#define QUEUE_SIZE 10

struct interrupt_transfer_queue_t {
    int endpoint_fd;
    bool *shutdown_flag;
    interrupt_transfer_t buffer[QUEUE_SIZE];
    int head;
    int tail;
};

void interrupt_transfer_queue_init(interrupt_transfer_queue_t *queue, int capacity) {
    queue->buffer = (interrupt_transfer_t *)malloc(capacity * sizeof(interrupt_transfer_t));
    if (queue->buffer == NULL) {
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
    interrupt_transfer_queue_t *queue = (interrupt_transfer_queue_t *)malloc(sizeof(interrupt_transfer_queue_t));
    if (queue == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for interrupt transfer queue.\n");
        exit(EXIT_FAILURE);
    }

    queue->endpoint_fd = endpoint_fd;
    queue->shutdown_flag = shutdown_flag;
    queue->head = 0;
    queue->tail = 0;

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
    pthread_mutex_lock(&queue->mutex);

    while (queue->head == NULL) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    *transfer = *(queue->head);

    if (queue->head == queue->tail) {
        queue->head = NULL;
        queue->tail = NULL;
    } else {
        queue->head = queue->head->next;
    }

    pthread_mutex_unlock(&queue->mutex);
}

void interrupt_transfer_queue_destroy(interrupt_transfer_queue_t *queue) {
    pthread_mutex_destroy(&queue->mutex);
    pthread_cond_destroy(&queue->cond);
}