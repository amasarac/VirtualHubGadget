#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>

#include "isochronous_queue.h"
#include "interrupt_transfer_queue.h"
#include "bulk_transfer_queue.h"

/* Simple representation of a connected device.  */
typedef struct {
    int id;
} device_t;

/* For this stub we expose a single static device.  */
static device_t devices[1] = { { 0 } };

/* Return a pointer to a connected device or NULL if the index is invalid.  */
device_t *get_connected_device(int index)
{
    if (index < 0 || index >= (int)(sizeof(devices)/sizeof(devices[0])))
        return NULL;
    return &devices[index];
}

static void *isochronous_transfer_sender(void *arg)
{
    isochronous_transfer_queue_t *queue = arg;
    isochronous_transfer_t transfer;
    while (true) {
        isochronous_transfer_queue_dequeue(queue, &transfer);
        /* Normally we would forward data here. */
        (void)transfer;
    }
    return NULL;
}

static void *interrupt_transfer_sender(void *arg)
{
    interrupt_transfer_queue_t *queue = arg;
    interrupt_transfer_t transfer;
    while (true) {
        interrupt_transfer_queue_dequeue(queue, &transfer);
        (void)transfer;
    }
    return NULL;
}

static void *bulk_transfer_sender(void *arg)
{
    bulk_transfer_queue_t *queue = arg;
    transfer_request_t request;
    while (true) {
        bulk_transfer_queue_dequeue(queue, &request);
        (void)request;
    }
    return NULL;
}

int main(void)
{
    /* Initialize transfer queues with explicit capacities. */
    isochronous_transfer_queue_t iso_queue;
    isochronous_transfer_queue_init(&iso_queue, 8);

    interrupt_transfer_queue_t int_queue;
    interrupt_transfer_queue_init(&int_queue, 8);

    bulk_transfer_queue_t bulk_queue;
    transfer_request_t bulk_requests[8];
    bulk_transfer_queue_init(&bulk_queue, bulk_requests, 8);

    /* Spawn worker threads for each queue. */
    pthread_t iso_thread, int_thread, bulk_thread;
    pthread_create(&iso_thread, NULL, isochronous_transfer_sender, &iso_queue);
    pthread_create(&int_thread, NULL, interrupt_transfer_sender, &int_queue);
    pthread_create(&bulk_thread, NULL, bulk_transfer_sender, &bulk_queue);

    /* Stub event loop. Real implementation would handle USB events here. */
    while (true) {
        sleep(1);
    }

    return 0;
}
