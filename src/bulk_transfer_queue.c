#include <stdbool.h>
#include "bulk_transfer_queue.h"

void bulk_transfer_queue_init(bulk_transfer_queue_t *queue, transfer_request_t *requests, int size) {
    queue->requests = requests;
    queue->size = size;
    queue->head = 0;
    queue->tail = 0;
    queue->full = false;
}

bool bulk_transfer_queue_enqueue(bulk_transfer_queue_t *queue, const transfer_request_t *request) {
    if (queue->full) {
        return false;
    }

    queue->requests[queue->tail] = *request;
    queue->tail = (queue->tail + 1) % queue->size;

    if (queue->tail == queue->head) {
        queue->full = true;
    }

    return true;
}

bool bulk_transfer_queue_dequeue(bulk_transfer_queue_t *queue, transfer_request_t *request) {
    if (!queue->full && queue->head == queue->tail) {
        return false;
    }

    *request = queue->requests[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    queue->full = false;

    return true;
}
