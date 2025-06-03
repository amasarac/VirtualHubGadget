#include "control_transfer_queue.h"

void control_transfer_queue_init(control_transfer_queue_t *queue,
                                 control_transfer_t *buffer,
                                 int size) {
    queue->transfers = buffer;
    queue->size = size;
    queue->head = 0;
    queue->tail = 0;
    queue->full = false;
}

bool control_transfer_queue_enqueue(control_transfer_queue_t *queue,
                                    const control_transfer_t *transfer) {
    if (queue->full) {
        return false;
    }

    queue->transfers[queue->tail] = *transfer;
    queue->tail = (queue->tail + 1) % queue->size;

    if (queue->tail == queue->head) {
        queue->full = true;
    }

    return true;
}

bool control_transfer_queue_dequeue(control_transfer_queue_t *queue,
                                    control_transfer_t *transfer) {
    if (!queue->full && queue->head == queue->tail) {
        return false;
    }

    *transfer = queue->transfers[queue->head];
    queue->head = (queue->head + 1) % queue->size;
    queue->full = false;

    return true;
}

void control_transfer_queue_destroy(control_transfer_queue_t *queue) {
    /* No dynamic allocations to free for now */
    queue->transfers = NULL;
    queue->size = 0;
    queue->head = 0;
    queue->tail = 0;
    queue->full = false;
}
