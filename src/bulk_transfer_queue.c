#include <stdbool.h>
#include "bulk_transfer_queue.h"

void bulk_transfer_queue_init(bulk_transfer_queue_t *queue, transfer_request_t *requests, int size) {
    queue->requests = requests;
    queue->size = size;
    queue->head = 0;
    queue->tail = 0;
    queue->full = false;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
}

void bulk_transfer_queue_destroy(bulk_transfer_queue_t *queue) {
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
}

bool bulk_transfer_queue_enqueue(bulk_transfer_queue_t *queue, const transfer_request_t *request) {
    pthread_mutex_lock(&queue->lock);

    while (queue->full) {
        pthread_cond_wait(&queue->not_full, &queue->lock);
    }

    queue->requests[queue->tail] = *request;
    queue->tail = (queue->tail + 1) % queue->size;

    if (queue->tail == queue->head) {
        queue->full = true;
    }

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->lock);
    return true;
}

bool bulk_transfer_queue_dequeue(bulk_transfer_queue_t *queue, transfer_request_t *request) {
    pthread_mutex_lock(&queue->lock);

    while (!queue->full && queue->head == queue->tail) {
        pthread_cond_wait(&queue->not_empty, &queue->lock);
    }

    *request = queue->requests[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    queue->full = false;

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->lock);
    return true;
}
