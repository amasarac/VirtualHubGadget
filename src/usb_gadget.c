
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include "gadgetfs_api.h"
#include "usb_gadget.h"

/* Stub for USBIP structures to fix compile errors */
typedef struct { uint32_t command; uint32_t seqnum; uint32_t devid; uint32_t direction; uint32_t ep; uint32_t num; uint32_t len; } usbip_header_t;
typedef struct { usbip_header_t base; unsigned char data[0]; } usbip_packet_t;
#define USBIP_MAX_DATA_SIZE 4096
#define USBIP_IN_ENDPOINT 0x81
#define USBIP_OUT_ENDPOINT 0x01

/* Global handles for forwarding data. In a real system, these would be managed per-connection */
static libusb_device_handle *g_src_dev_handle = NULL;
static libusb_device_handle *g_dst_dev_handle = NULL;

void forward_isochronous_transfer(int config_index, int iface_index, int altsetting_index, int ep_index) {
    (void)config_index; (void)iface_index; (void)altsetting_index; (void)ep_index;
}

void forward_control_transfer(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue, uint16_t wIndex, unsigned char *data, uint16_t wLength) {
    if (!g_dst_dev_handle) return;
    int r = libusb_control_transfer(g_dst_dev_handle, bmRequestType, bRequest, wValue, wIndex, data, wLength, 1000);
    if (r < 0) {
        fprintf(stderr, "Error: control transfer failed: %s\n", libusb_error_name(r));
    }
}

void forward_bulk_transfer(int src_ep_addr, int dst_ep_addr) {
    if (!g_src_dev_handle || !g_dst_dev_handle) return;
    unsigned char data_buffer[4096];
    int actual_length;
    int r = libusb_bulk_transfer(g_src_dev_handle, src_ep_addr, data_buffer, sizeof(data_buffer), &actual_length, 1000);
    if (r < 0) {
        fprintf(stderr, "Error: bulk read failed: %s\n", libusb_error_name(r));
        return;
    }

    r = libusb_bulk_transfer(g_dst_dev_handle, dst_ep_addr, data_buffer, actual_length, &actual_length, 1000);
    if (r < 0) {
        fprintf(stderr, "Error: bulk write failed: %s\n", libusb_error_name(r));
    }
}

void forward_interrupt_transfer(int src_ep_addr, int dst_ep_addr) {
    if (!g_src_dev_handle || !g_dst_dev_handle) return;
    unsigned char data_buffer[64];
    int actual_length;
    int r = libusb_interrupt_transfer(g_src_dev_handle, src_ep_addr, data_buffer, sizeof(data_buffer), &actual_length, 1000);
    if (r < 0) {
        fprintf(stderr, "Error: interrupt read failed: %s\n", libusb_error_name(r));
        return;
    }

    r = libusb_interrupt_transfer(g_dst_dev_handle, dst_ep_addr, data_buffer, actual_length, &actual_length, 1000);
    if (r < 0) {
        fprintf(stderr, "Error: interrupt write failed: %s\n", libusb_error_name(r));
    }
}

void forward_data(usb_transfer_t *transfer) {
    if (!transfer || !g_dst_dev_handle) return;

    unsigned char *data = transfer->data;
    int data_length = transfer->length;
    int actual_length = 0;
    int result = 0;

    switch (transfer->transfer_type) {
        case CONTROL_TRANSFER:
            if (data_length >= 8) {
                result = libusb_control_transfer(g_dst_dev_handle, transfer->endpoint, data[0], data[1] | (data[2] << 8), data[3] | (data[4] << 8), data + 8, data_length - 8, 5000);
            }
            break;
        case BULK_TRANSFER:
            result = libusb_bulk_transfer(g_dst_dev_handle, transfer->endpoint, data, data_length, &actual_length, 5000);
            break;
        case INTERRUPT_TRANSFER:
            result = libusb_interrupt_transfer(g_dst_dev_handle, transfer->endpoint, data, data_length, &actual_length, 5000);
            break;
        case ISOCHRONOUS_TRANSFER:
            fprintf(stderr, "Isochronous forwarding not supported\n");
            break;
        default:
            fprintf(stderr, "Invalid transfer type: %d\n", transfer->transfer_type);
            return;
    }

    if (result < 0 && transfer->transfer_type != ISOCHRONOUS_TRANSFER) {
        fprintf(stderr, "Error in forward_data: %s\n", libusb_error_name(result));
    }
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
    (void)device;
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
