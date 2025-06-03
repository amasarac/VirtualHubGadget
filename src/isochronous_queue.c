//isochronous_queue.c

#include "isochronous_queue.h"
#include <stdlib.h>
#include <string.h>

void isochronous_transfer_queue_init(isochronous_transfer_queue_t *queue, size_t capacity) {
    queue->capacity = capacity;
    queue->size = 0;
    queue->front = 0;
    queue->rear = 0;
    queue->queue = malloc(capacity * sizeof(isochronous_transfer_t));
    pthread_mutex_init(&queue->mutex, NULL);
    pthread_cond_init(&queue->cond_full, NULL);
    pthread_cond_init(&queue->cond_empty, NULL);
}


void isochronous_transfer_queue_destroy(isochronous_transfer_queue_t *q) {
    if (q->queue) {
        free(q->queue);
        q->queue = NULL;
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

    queue->queue[queue->rear] = transfer;
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

    *transfer = queue->queue[queue->front];
    queue->front = (queue->front + 1) % queue->capacity;
    queue->size--;

    pthread_cond_signal(&queue->cond_full);
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}


void *isochronous_transfer_sender(void *arg) {
    isochronous_transfer_queue_t *queue = (isochronous_transfer_queue_t *)arg;
    struct timespec sleep_interval;

    while (1) {
        isochronous_transfer_t transfer;
        isochronous_transfer_queue_dequeue(queue, &transfer);

        // Send the transfer using GadgetFS API
        ssize_t bytes_sent = write(transfer.endpoint_fd, transfer.data, transfer.length);

        if (bytes_sent == -1) {
            perror("Error sending isochronous transfer");
            continue;
        }

        // Calculate sleep interval based on the isochronous transfer rate
        int transfer_rate = transfer.rate;
        sleep_interval.tv_sec = 0;
        sleep_interval.tv_nsec = transfer_rate * 1000;

        // Sleep until the next isochronous transfer
        nanosleep(&sleep_interval, NULL);
    }

    return NULL;
}

