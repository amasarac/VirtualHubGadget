#ifndef MAIN_H
#define MAIN_H

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include "usb_device.h"
#include "usb_interface.h"
#include "transfer.h"
#include "isochronous_queue.h"
#include "interrupt_transfer_queue.h"
#include "bulk_transfer_queue.h"
#include "cli.h"

#define MAX_DEVICES 10
#define MAX_TRANSFER_SIZE 1024

typedef struct {
    usb_device_t *device;
    usb_interface_t *interface;
} device_interface_t;

extern device_interface_t device_interfaces[MAX_DEVICES];
extern int num_devices;

// Function declarations
void handle_signal(int signum);
void *isochronous_transfer_sender(void *arg);
void *interrupt_transfer_sender(void *arg);
void *bulk_transfer_sender(void *arg);
void *transfer_receiver(void *arg);

#endif /* MAIN_H */
