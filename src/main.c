#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>

#include "main.h"
#include "isochronous_queue.h"
#include "interrupt_transfer_queue.h"
#include "bulk_transfer_queue.h"
#include "cli.h"
#include "common.h"

static volatile bool running = true;

device_interface_t device_interfaces[MAX_DEVICES];
int num_devices = 0;

void handle_signal(int signum) {
    (void)signum;
    running = false;
}

/* Simple representation of a connected device. */
typedef struct {
    int id;
} device_t;

static device_t devices[1] = { { 0 } };

device_t *get_connected_device(int index) {
    if (index < 0 || index >= (int)(sizeof(devices) / sizeof(devices[0])))
        return NULL;
    return &devices[index];
}

void *isochronous_transfer_sender(void *arg) {
    isochronous_transfer_queue_t *queue = (isochronous_transfer_queue_t *)arg;
    isochronous_transfer_t transfer;
    while (running) {
        if (isochronous_transfer_queue_dequeue(queue, &transfer) == 0) {
            if (transfer.endpoint_fd >= 0 && transfer.data && transfer.length > 0) {
                ssize_t ret = write(transfer.endpoint_fd, transfer.data, transfer.length);
                (void)ret;
            }
        }
    }
    return NULL;
}

void *interrupt_transfer_sender(void *arg) {
    interrupt_transfer_queue_t *queue = (interrupt_transfer_queue_t *)arg;
    interrupt_transfer_t transfer;
    while (running) {
        interrupt_transfer_queue_dequeue(queue, &transfer);
        if (transfer.endpoint_fd >= 0 && transfer.data && transfer.length > 0) {
            ssize_t ret = write(transfer.endpoint_fd, transfer.data, transfer.length);
            (void)ret;
        }
    }
    return NULL;
}

void *bulk_transfer_sender(void *arg) {
    bulk_transfer_queue_t *queue = (bulk_transfer_queue_t *)arg;
    transfer_request_t request;
    while (running) {
        if (bulk_transfer_queue_dequeue(queue, &request)) {
            if (request.data && request.length > 0) {
                /* Process bulk transfer request */
                (void)request;
            }
        }
    }
    return NULL;
}

int main(int argc, char **argv) {
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    if (argc > 1) {
        cli_t *cli = NULL;
        cli_init(&cli);

        if (argc > 2 && strcmp(argv[1], "connect") == 0) {
            cli_connect(cli, argv[2]);
        } else if (argc > 1 && strcmp(argv[1], "disconnect") == 0) {
            cli_disconnect(cli);
        } else {
            printf("Usage: %s connect <server> | disconnect\n", argv[0]);
        }

        cli_cleanup(cli);
        free(cli);
        return 0;
    }

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

    printf("VirtualHubGadget running. Press Ctrl+C to exit.\n");

    /* Main event loop. */
    while (running) {
        sleep(1);
    }

    printf("Shutting down VirtualHubGadget...\n");

    pthread_cancel(iso_thread);
    pthread_cancel(int_thread);
    pthread_cancel(bulk_thread);

    pthread_join(iso_thread, NULL);
    pthread_join(int_thread, NULL);
    pthread_join(bulk_thread, NULL);

    isochronous_transfer_queue_destroy(&iso_queue);
    interrupt_transfer_queue_destroy(&int_queue);
    bulk_transfer_queue_destroy(&bulk_queue);

    return 0;
}
