#ifndef BULK_TRANSFER_QUEUE_H
#define BULK_TRANSFER_QUEUE_H

#include <stdbool.h>
#include <pthread.h>
#include <stdlib.h>
#include "transfer.h"
#include "transfer_request.h"

typedef struct {
    transfer_request_t *requests;
    int size;
    int head;
    int tail;
    bool full;
} bulk_transfer_queue_t;

void bulk_transfer_queue_init(bulk_transfer_queue_t *queue, transfer_request_t *requests, int size);
bool bulk_transfer_queue_enqueue(bulk_transfer_queue_t *queue, const transfer_request_t *request);
bool bulk_transfer_queue_dequeue(bulk_transfer_queue_t *queue, transfer_request_t *request);

#endif /* BULK_TRANSFER_QUEUE_H */
