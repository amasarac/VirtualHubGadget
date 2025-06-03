//isochronous_queue.c

#include "isochronous_queue.h"
#include <stdlib.h>
#include <string.h>

void isochronous_transfer_queue_init(isochronous_transfer_queue_t *queue, size_t capacity) {
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = 0;
    queue->transfers = malloc(capacity * sizeof(isochronous_transfer_t));
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond, NULL);
}


void isochronous_transfer_queue_destroy(isochronous_transfer_queue_t *q) {
    // Free any memory allocated for the queue
    if (q->transfers) {
        free(q->transfers);
        q->transfers = NULL;
    }

    // Set the queue size to zero
    q->size = 0;
}


int isochronous_transfer_queue_enqueue(isochronous_transfer_queue_t *queue, isochronous_transfer_t transfer) {
    pthread_mutex_lock(&queue->mutex);

    if (queue->size == queue->capacity) {
        pthread_mutex_unlock(&queue->mutex);
        return -1; // Queue is full
    }

    queue->transfers[queue->rear] = transfer;
    queue->rear = (queue->rear + 1) % queue->capacity;
    queue->size++;

    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}


int isochronous_transfer_queue_dequeue(isochronous_transfer_queue_t *queue, isochronous_transfer_t *transfer) {
    pthread_mutex_lock(&queue->mutex);

    while (queue->size == 0) {
        pthread_cond_wait(&queue->cond, &queue->mutex);
    }

    *transfer = queue->transfers[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    pthread_mutex_unlock(&queue->mutex);
    return 0;
}


#if 0
/*
 * This helper was intended to pop transfers from the queue and send them
 * through GadgetFS.  The current transfer structure used in the project
 * does not provide the required fields, so keep the implementation
 * disabled to avoid compilation issues in the unit tests.
 */
void *isochronous_transfer_sender(void *arg);
#endif

