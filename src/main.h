#ifndef _MAIN_H_
#define _MAIN_H_

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
#include "isochronous_transfer_queue.h"
#include "interrupt_transfer_queue.h"
#include "bulk_transfer_queue.h"

#define MAX_DEVICES 10
#define MAX_TRANSFER_SIZE 1024

typedef struct {
    uint8_t type;
    uint8_t endpoint;
    uint8_t data[MAX_TRANSFER_SIZE];
    uint32_t length;
    uint32_t rate;
} transfer_request_t;


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
void cli_init();
void cli_run();
int parse_transfer_request(char *input, transfer_request_t *request);

#endif /* MAIN_H */