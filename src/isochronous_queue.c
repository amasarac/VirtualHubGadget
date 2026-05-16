#include "isochronous_queue.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

void isochronous_transfer_queue_init(isochronous_transfer_queue_t *queue, size_t capacity) {
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = 0;
    queue->transfers = malloc(capacity * sizeof(isochronous_transfer_t));
    if (!queue->transfers) {
        fprintf(stderr, "Failed to allocate memory for isochronous transfer queue\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond_full, NULL);
    pthread_cond_init(&queue->cond_empty, NULL);
}

void isochronous_transfer_queue_destroy(isochronous_transfer_queue_t *q) {
    if (q->transfers) {
        free(q->transfers);
        q->transfers = NULL;
    }

    q->size = 0;
    pthread_mutex_destroy(&q->mutex);
    pthread_cond_destroy(&q->cond_full);
    pthread_cond_destroy(&q->cond_empty);
}

int isochronous_transfer_queue_enqueue(isochronous_transfer_queue_t *queue, isochronous_transfer_t transfer) {
    pthread_mutex_lock(&queue->mutex);

    while (queue->size == queue->capacity) {
        pthread_cond_wait(&queue->cond_full, &queue->mutex);
    }

    queue->transfers[queue->rear] = transfer;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;

    pthread_cond_signal(&queue->cond_empty);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

int isochronous_transfer_queue_dequeue(isochronous_transfer_queue_t *queue, isochronous_transfer_t *transfer) {
    pthread_mutex_lock(&queue->mutex);

    while (queue->size == 0) {
        pthread_cond_wait(&queue->cond_empty, &queue->mutex);
    }

    *transfer = queue->transfers[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    pthread_cond_signal(&queue->cond_full);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}
