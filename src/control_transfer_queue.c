#include "control_transfer_queue.h"

void control_transfer_queue_init(control_transfer_queue_t *queue,
                                 control_transfer_t *buffer,
                                 int size) {
    queue->transfers = buffer;
    queue->size = size;
    queue->head = 0;
    queue->tail = 0;
    queue->full = false;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->not_empty, NULL);
    pthread_cond_init(&queue->not_full, NULL);
}

bool control_transfer_queue_enqueue(control_transfer_queue_t *queue,
                                    const control_transfer_t *transfer) {
    pthread_mutex_lock(&queue->lock);

    while (queue->full) {
        pthread_cond_wait(&queue->not_full, &queue->lock);
    }

    queue->transfers[queue->tail] = *transfer;
    queue->tail = (queue->tail + 1) % queue->size;

    if (queue->tail == queue->head) {
        queue->full = true;
    }

    pthread_cond_signal(&queue->not_empty);
    pthread_mutex_unlock(&queue->lock);
    return true;
}

bool control_transfer_queue_dequeue(control_transfer_queue_t *queue,
                                    control_transfer_t *transfer) {
    pthread_mutex_lock(&queue->lock);

    while (!queue->full && queue->head == queue->tail) {
        pthread_cond_wait(&queue->not_empty, &queue->lock);
    }

    *transfer = queue->transfers[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    queue->full = false;

    pthread_cond_signal(&queue->not_full);
    pthread_mutex_unlock(&queue->lock);
    return true;
}

void control_transfer_queue_destroy(control_transfer_queue_t *queue) {
    pthread_mutex_destroy(&queue->lock);
    pthread_cond_destroy(&queue->not_empty);
    pthread_cond_destroy(&queue->not_full);
    queue->transfers = NULL;
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
    queue->full = false;
}
