#ifndef CONTROL_TRANSFER_QUEUE_H
#define CONTROL_TRANSFER_QUEUE_H

#include <stdbool.h>
#include "transfer_request.h"

typedef struct {
    control_transfer_t *transfers;
    int size;
    int head;
    int tail;
    bool full;
} control_transfer_queue_t;

void control_transfer_queue_init(control_transfer_queue_t *queue,
                                 control_transfer_t *buffer,
                                 int size);
bool control_transfer_queue_enqueue(control_transfer_queue_t *queue,
                                    const control_transfer_t *transfer);
bool control_transfer_queue_dequeue(control_transfer_queue_t *queue,
                                    control_transfer_t *transfer);
void control_transfer_queue_destroy(control_transfer_queue_t *queue);

#endif /* CONTROL_TRANSFER_QUEUE_H */
