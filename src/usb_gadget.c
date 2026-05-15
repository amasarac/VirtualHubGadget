#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "gadgetfs_api.h"
#include "usb_gadget.h"

/* Stub for USBIP structures to fix compile errors */
typedef struct { uint32_t command; uint32_t seqnum; uint32_t devid; uint32_t direction; uint32_t ep; uint32_t num; uint32_t len; } usbip_header_t;
typedef struct { usbip_header_t base; unsigned char data[0]; } usbip_packet_t;
#define USBIP_MAX_DATA_SIZE 4096
#define USBIP_IN_ENDPOINT 0x81
#define USBIP_OUT_ENDPOINT 0x01

void forward_isochronous_transfer(int config_index, int iface_index, int altsetting_index, int ep_index) {
    (void)config_index; (void)iface_index; (void)altsetting_index; (void)ep_index;
}

void forward_control_transfer() {}

void forward_bulk_transfer(int src_ep_addr, int dst_ep_addr) {
    (void)src_ep_addr; (void)dst_ep_addr;
}

void forward_interrupt_transfer(int src_ep_addr, int dst_ep_addr) {
    (void)src_ep_addr; (void)dst_ep_addr;
}

void forward_data(usb_transfer_t *transfer) {
    (void)transfer;
}

static void *handle_gadgetfs_events(void *arg) {
    int gadgetfs_fd = *(int *)arg;

    while (1) {
        int poll_result = gadgetfs_poll_fd(gadgetfs_fd);
        if (poll_result > 0) {
            int event_type = gadgetfs_event(gadgetfs_fd, NULL);
            switch (event_type) {
                case GADGETFS_NOP: break;
                case GADGETFS_CONNECT: printf("Device connected\n"); break;
                case GADGETFS_DISCONNECT: printf("Device disconnected\n"); break;
                case GADGETFS_SETUP: printf("Setup packet received\n"); break;
                case GADGETFS_SUSPEND: printf("Device suspended\n"); break;
                default: fprintf(stderr, "Unknown GadgetFS event: %d\n", event_type); break;
            }
        } else if (poll_result < 0) {
            perror("Error polling GadgetFS");
            break;
        }
    }
    return NULL;
}

void *handle_usbip_traffic(void *arg) {
    (void)arg;
    return NULL;
}

int usb_gadget_start(const char *gadgetfs_dir, libusb_device *device) {
    if (!gadgetfs_dir || !device) {
        return -1;
    }

    usb_device_info_t device_info;
    memset(&device_info, 0, sizeof(device_info));

    gadgetfs_t gfs;
    if (gadgetfs_init(&gfs, gadgetfs_dir, &device_info) < 0) {
        perror("Error initializing GadgetFS");
        return -1;
    }

    pthread_t gadgetfs_thread;
    int thread_create_result = pthread_create(&gadgetfs_thread, NULL, handle_gadgetfs_events, &gfs.fd);
    if (thread_create_result != 0) {
        perror("Error creating GadgetFS event handling thread");
        gadgetfs_exit(&gfs);
        return -1;
    }

    pthread_t usbip_thread;
    thread_create_result = pthread_create(&usbip_thread, NULL, handle_usbip_traffic, device);
    if (thread_create_result != 0) {
        perror("Error creating USB/IP handling thread");
        pthread_cancel(gadgetfs_thread);
        gadgetfs_exit(&gfs);
        return -1;
    }

    return 0;
}
