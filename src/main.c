//main.c

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include "isochronous_transfer_queue.h"
#include "interrupt_transfer_queue.h"
#include "bulk_transfer_queue.h"
#include "common.h"

typedef enum {
    CONNECTION_NONE,
    CONNECTION_IN_PROGRESS,
    CONNECTION_ESTABLISHED
} connection_state_t;

typedef struct {
    connection_state_t usbip_connection_state;
    // other members...
} cli_t;

int parse_transfer_request(char *request_str, transfer_request_t *transfer_request) {
    // Tokenize the request string using strtok() function
    char *token = strtok(request_str, ",");
    if (token == NULL) {
        // Invalid request
        return -1;
    }

    // Parse transfer type
    if (strcmp(token, "isochronous") == 0) {
        transfer_request->transfer_type = TRANSFER_TYPE_ISOCHRONOUS;
    } else if (strcmp(token, "interrupt") == 0) {
        transfer_request->transfer_type = TRANSFER_TYPE_INTERRUPT;
    } else if (strcmp(token, "bulk") == 0) {
        transfer_request->transfer_type = TRANSFER_TYPE_BULK;
    } else {
        // Invalid transfer type
        return -1;
    }

    // Parse device ID
    token = strtok(NULL, ",");
    if (token == NULL) {
        // Invalid request
        return -1;
    }
    transfer_request->device_id = atoi(token);

    // Parse endpoint address
    token = strtok(NULL, ",");
    if (token == NULL) {
        // Invalid request
        return -1;
    }
    transfer_request->endpoint_address = atoi(token);

    // Parse transfer rate (only for isochronous transfers)
    if (transfer_request->transfer_type == TRANSFER_TYPE_ISOCHRONOUS) {
        token = strtok(NULL, ",");
        if (token == NULL) {
            // Invalid request
            return -1;
        }
        transfer_request->rate = atoi(token);
    }

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
            // Handle error
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

void *interrupt_transfer_sender(void *arg) {
    interrupt_transfer_queue_t *queue = (interrupt_transfer_queue_t *)arg;

    while (1) {
        interrupt_transfer_t transfer;
        interrupt_transfer_queue_dequeue(queue, &transfer);

        // Send the transfer using GadgetFS API
        ssize_t bytes_sent = write(transfer.endpoint_fd, transfer.data, transfer.length);

        if (bytes_sent == -1) {
            // Handle error
        }
    }

    return NULL;
}

void *bulk_transfer_sender(void *arg) {
    bulk_transfer_queue_t *queue = (bulk_transfer_queue_t *)arg;

    while (1) {
        bulk_transfer_t transfer;
        bulk_transfer_queue_dequeue(queue, &transfer);

        // Send the transfer using GadgetFS API
        ssize_t bytes_sent = write(transfer.endpoint_fd, transfer.data, transfer.length);

        if (bytes_sent == -1) {
            // Handle error
        }
    }

    return NULL;
}

int main() {
    cli_t *cli = calloc(1, sizeof(cli_t));
    cli->usbip_connection_state = CONNECTION_NONE;

    // Initialize queues for each transfer type
    isochronous_transfer_queue_t isoch_queue;
    isochronous_transfer_queue_init(&isoch_queue);

    interrupt_transfer_queue_t intr_queue;
    interrupt_transfer_queue_init(&intr_queue);

    bulk_transfer_queue_t bulk_queue;
    bulk_transfer_queue_init(&bulk_queue);

    // Loop through all connected devices and spawn a new thread for each transfer type
    for (int i = 0; i < num_connected_devices; i++) {
        device_t *device = get_connected_device(i);

        // Spawn thread for isochronous transfers
        pthread_t isoch_thread;
        pthread_create(&isoch_thread, NULL, isochronous_transfer_sender, &isoch_queue);

        // Spawn thread for interrupt transfers
        pthread_t intr_thread;
        pthread_create(&intr_thread, NULL, interrupt_transfer_sender, &intr_queue);

        // Spawn thread for bulk transfers
        pthread_t bulk_thread;
        pthread_create(&bulk_thread, NULL, bulk_transfer_sender, &bulk_queue);

        // Wait for threads to start
    // Wait for threads to start
    for (i = 0; i < num_devices; i++) {
        while (!threads_started[i]) {
            usleep(1000);
        }
    }

    // Main loop to receive transfers from the host
    while (1) {
        // Receive a transfer request from the host using GadgetFS API
        ssize_t bytes_received = read(ep0_fd, buffer, MAX_TRANSFER_SIZE);

        if (bytes_received == -1) {
            // Handle error
            perror("Error reading from ep0");
            break;
        }

        // Parse the transfer request and create a transfer object
        transfer_t transfer = parse_transfer_request(buffer, bytes_received);

        // Enqueue the transfer object to the appropriate queue based on the transfer type
        if (transfer.type == CONTROL_TRANSFER) {
            control_transfer_queue_enqueue(&control_transfer_queues[transfer.device], transfer);
        } else if (transfer.type == ISOCHRONOUS_TRANSFER) {
            isochronous_transfer_queue_enqueue(&isochronous_transfer_queues[transfer.device], transfer);
        } else if (transfer.type == INTERRUPT_TRANSFER) {
            interrupt_transfer_queue_enqueue(&interrupt_transfer_queues[transfer.device], transfer);
        } else if (transfer.type == BULK_TRANSFER) {
            bulk_transfer_queue_enqueue(&bulk_transfer_queues[transfer.device], transfer);
        } else {
            // Handle error
            printf("Invalid transfer type: %d\n", transfer.type);
            break;
        }
    }

    // Clean up resources
    for (i = 0; i < num_devices; i++) {
        pthread_cancel(transfer_threads[i]);
        pthread_join(transfer_threads[i], NULL);
        control_transfer_queue_destroy(&control_transfer_queues[i]);
        isochronous_transfer_queue_destroy(&isochronous_transfer_queues[i]);
        interrupt_transfer_queue_destroy(&interrupt_transfer_queues[i]);
        bulk_transfer_queue_destroy(&bulk_transfer_queues[i]);
        close(device_fds[i]);
    }
    free(device_fds);
    free(control_transfer_queues);
    free(isochronous_transfer_queues);
    free(interrupt_transfer_queues);
    free(bulk_transfer_queues);
    free(threads_started);

    return 0;
}
