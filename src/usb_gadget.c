//usbgadget.c

#include <stdio.h>
#include <pthread.h>
#include "usb_gadget.h"
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include "gadgetfs_api.h"

libusb_device_handle *src_dev_handle; // Source device handle
libusb_device_handle *dst_dev_handle; // Destination device handle

void forward_isochronous_transfer(int src_ep_addr, int dst_ep_addr) {
    // Get the number of configurations of the USB device
int num_configurations = dev->config->bNumInterfaces;

// Iterate over the configurations
for (int config_index = 0; config_index < num_configurations; config_index++) {
    const struct libusb_interface *interface = &dev->config->interface[config_index];
    
    // Iterate over the interface's alternate settings
    for (int alt_setting_index = 0; alt_setting_index < interface->num_altsetting; alt_setting_index++) {
        const struct libusb_interface_descriptor *interface_desc = &interface->altsetting[alt_setting_index];

        // Iterate over the endpoints in the interface descriptor
        for (int ep_index = 0; ep_index < interface_desc->bNumEndpoints; ep_index++) {
            const struct libusb_endpoint_descriptor *ep_desc = &interface_desc->endpoint[ep_index];

            // Check if the endpoint is an isochronous endpoint
            if ((ep_desc->bmAttributes & LIBUSB_TRANSFER_TYPE_MASK) == LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
                int num_iso_packets = ep_desc->wMaxPacketSize & 0x07FF; // Get the number of iso packets
                int packet_length = (ep_desc->wMaxPacketSize >> 11) + 1; // Get the packet length

                // Print the information
                printf("Endpoint %d: num_iso_packets=%d, packet_length=%d\n", ep_index, num_iso_packets, packet_length);
            }
        }
    }
}

    // Set up libusb_transfer structures for both source and destination devices
    struct libusb_transfer *src_transfer = libusb_alloc_transfer(num_iso_packets);
    struct libusb_transfer *dst_transfer = libusb_alloc_transfer(num_iso_packets);

    if (!src_transfer || !dst_transfer) {
        fprintf(stderr, "Error: failed to allocate transfers\n");
        libusb_free_transfer(src_transfer);
        libusb_free_transfer(dst_transfer);
        return;
    }

    unsigned char data_buffer[num_iso_packets * packet_length];

    // Fill the source transfer structure
    libusb_fill_iso_transfer(src_transfer, src_dev_handle, src_ep_addr, data_buffer,
                             num_iso_packets * packet_length, num_iso_packets, NULL, NULL, 0);

    // Fill the destination transfer structure
    libusb_fill_iso_transfer(dst_transfer, dst_dev_handle, dst_ep_addr, data_buffer,
                             num_iso_packets * packet_length, num_iso_packets, NULL, NULL, 0);

    // Set up the libusb_iso_packet_descriptor structures for both transfers
    libusb_set_iso_packet_lengths(src_transfer, packet_length);
    libusb_set_iso_packet_lengths(dst_transfer, packet_length);

    // Submit the source transfer and wait for it to complete
    int r = libusb_submit_transfer(src_transfer);
    if (r < 0) {
        fprintf(stderr, "Error: failed to submit source transfer\n");
        libusb_free_transfer(src_transfer);
        libusb_free_transfer(dst_transfer);
        return;
    }

    r = libusb_handle_events_completed(NULL, NULL);
    if (r < 0) {
        fprintf(stderr, "Error: failed to handle source transfer events\n");
        libusb_cancel_transfer(src_transfer);
        libusb_free_transfer(src_transfer);
        libusb_free_transfer(dst_transfer);
        return;
    }

    // Submit the destination transfer and wait for it to complete
    r = libusb_submit_transfer(dst_transfer);
    if (r < 0) {
        fprintf(stderr, "Error: failed to submit destination transfer\n");
        libusb_free_transfer(src_transfer);
        libusb_free_transfer(dst_transfer);
        return;
    }

    r = libusb_handle_events_completed(NULL, NULL);
    if (r < 0) {
        fprintf(stderr, "Error: failed to handle destination transfer events\n");
        libusb_cancel_transfer(dst_transfer);
        libusb_free_transfer(src_transfer);
        libusb_free_transfer(dst_transfer);
        return;
    }

    // Free the transfers
    libusb_free_transfer(src_transfer);
    libusb_free_transfer(dst_transfer);
}

// Forward control transfer
void forward_control_transfer() {
    // You would get these values from the source device's control transfer
    uint8_t bmRequestType = ...;
    uint8_t bRequest = ...;
    uint16_t wValue = ...;
    uint16_t wIndex = ...;

    unsigned char data_buffer[64];
    uint16_t wLength = sizeof(data_buffer);
    unsigned int timeout = 1000;

    // Perform the control transfer on the destination device
    int r = libusb_control_transfer(dst_dev_handle, bmRequestType, bRequest, wValue, wIndex, data_buffer, wLength, timeout);
    if (r < 0) {
        fprintf(stderr, "Error: control transfer failed\n");
        return;
    }
}

// Forward bulk transfer
void forward_bulk_transfer(int src_ep_addr, int dst_ep_addr) {
    unsigned char data_buffer[64];
    int actual_length;
    int r = libusb_bulk_transfer(src_dev_handle, src_ep_addr, data_buffer, sizeof(data_buffer), &actual_length, 1000);
    if (r < 0) {
        fprintf(stderr, "Error: bulk read failed\n");
        return;
    }

    r = libusb_bulk_transfer(dst_dev_handle, dst_ep_addr, data_buffer, actual_length, &actual_length, 1000);
    if (r < 0) {
        fprintf(stderr, "Error: bulk write failed\n");
        return;
    }
}

// Forward interrupt transfer
void forward_interrupt_transfer(int src_ep_addr, int dst_ep_addr) {
    unsigned char data_buffer[64];
    int actual_length;
    int r = libusb_interrupt_transfer(src_dev_handle, src_ep_addr, data_buffer, sizeof(data_buffer), &actual_length, 1000);
    if (r < 0) {
        fprintf(stderr, "Error: interrupt read failed\n");
        return;
    }

    r = libusb_interrupt_transfer(dst_dev_handle, dst_ep_addr, data_buffer, actual_length, &actual_length, 1000);
    if (r < 0) {
        fprintf(stderr, "Error: interrupt write failed\n");
        return;
    }
}

void forward_data(usb_transfer_t *transfer) {
    libusb_device_handle *handle = /* Retrieve the libusb_device_handle for the device */;
    unsigned char *data = /* Pointer to the data buffer */;
    int data_length = /* Length of the data buffer */;
    int result;

    switch (transfer->transfer_type) {
        case LIBUSB_TRANSFER_TYPE_CONTROL:
            // Perform control transfer
            result = libusb_control_transfer(handle, transfer->endpoint_address, data[0], data[1] | (data[2] << 8), data[3] | (data[4] << 8), data + 8, data_length - 8, 5000);
            break;
        case LIBUSB_TRANSFER_TYPE_BULK:
            // Perform bulk transfer
            result = libusb_bulk_transfer(handle, transfer->endpoint_address, data, data_length, &actual_length, 5000);
            break;
        case LIBUSB_TRANSFER_TYPE_INTERRUPT:
            // Perform interrupt transfer
            result = libusb_interrupt_transfer(handle, transfer->endpoint_address, data, data_length, &actual_length, 5000);
            break;
        case LIBUSB_TRANSFER_TYPE_ISOCHRONOUS:
            // Perform isochronous transfer
            struct libusb_transfer *iso_transfer = libusb_alloc_transfer(transfer->num_iso_packets);
            if (!iso_transfer) {
                fprintf(stderr, "Error allocating isochronous transfer\n");
                return;
            }

            libusb_fill_iso_transfer(iso_transfer, handle, transfer->endpoint_address, data, data_length, transfer->num_iso_packets, isochronous_transfer_callback, NULL, 5000);
            libusb_set_iso_packet_lengths(iso_transfer, transfer->packet_length);
            result = libusb_submit_transfer(iso_transfer);

            if (result < 0) {
                fprintf(stderr, "Error submitting isochronous transfer: %s\n", libusb_error_name(result));
                libusb_free_transfer(iso_transfer);
            }
            break;
        default:
            fprintf(stderr, "Invalid transfer type: %d\n", transfer->transfer_type);
            return;
    }

    if (result < 0 && transfer->transfer_type != LIBUSB_TRANSFER_TYPE_ISOCHRONOUS) {
        fprintf(stderr, "Error in forward_data: %s\n", libusb_error_name(result));
    }
}

static void *handle_gadgetfs_events(void *arg) {
    int gadgetfs_fd = *(int *)arg;

    while (1) {
        int poll_result = gadgetfs_poll_fd(gadgetfs_fd);
        if (poll_result > 0) {
            // Handle GadgetFS events here
            int event_type = gadgetfs_event(gadgetfs_fd, NULL);
            switch (event_type) {
                case GADGETFS_NOP:
                    break;
                case GADGETFS_CONNECT:
                    printf("Device connected\n");
                    break;
                case GADGETFS_DISCONNECT:
                    printf("Device disconnected\n");
                    break;
                case GADGETFS_SETUP:
                    printf("Setup packet received\n");
                    break;
                case GADGETFS_SUSPEND:
                    printf("Device suspended\n");
                    break;
                case GADGETFS_RESUME:
                    printf("Device resumed\n");
                    break;
                default:
                    fprintf(stderr, "Unknown GadgetFS event: %d\n", event_type);
                    break;
            }
        } else if (poll_result < 0) {
            perror("Error polling GadgetFS");
            break;
        }
    }

    return NULL;
}

void *handle_usbip_traffic(void *arg) {
    libusb_device *device = (libusb_device *)arg;

    // Initialize USB/IP traffic handling
    libusb_context *context;
    int result = libusb_init(&context);
    if (result < 0) {
        fprintf(stderr, "Error initializing libusb: %s\n", libusb_error_name(result));
        return NULL;
    }

    libusb_device_handle *handle = libusb_open_device_with_vid_pid(
        context,
        libusb_get_device_vendor_id(device),
        libusb_get_device_product_id(device));
    if (!handle) {
        fprintf(stderr, "Error opening device with USB/IP: %s\n", libusb_error_name(result));
        libusb_exit(context);
        return NULL;
    }

    result = libusb_claim_interface(handle, 0);
    if (result < 0) {
        fprintf(stderr, "Error claiming interface with USB/IP: %s\n", libusb_error_name(result));
        libusb_close(handle);
        libusb_exit(context);
        return NULL;
    }

    while (1) {
        // Process incoming USB/IP traffic
        unsigned char in_buffer[USBIP_MAX_DATA_SIZE];
        int actual_length;

        result = libusb_bulk_transfer(handle, USBIP_IN_ENDPOINT, in_buffer, sizeof(in_buffer), &actual_length, 0);
        if (result < 0) {
            fprintf(stderr, "Error receiving USB/IP packet: %s\n", libusb_error_name(result));
            break;
        }

        // Forward the incoming packet to the appropriate endpoint
        usbip_packet_t *in_packet = (usbip_packet_t *)in_buffer;
        usb_transfer_t transfer;
        transfer.endpoint_address = in_packet->base.ep;
        transfer.transfer_type = in_packet->base.type;
        transfer.num_iso_packets = in_packet->base.num;
        transfer.packet_length = in_packet->base.len;
        transfer.timeout = 5000;
        memcpy(transfer.data, in_packet->data, actual_length - sizeof(usbip_header_t));
        forward_data(&transfer);

        // Send outgoing USB/IP traffic
        unsigned char out_buffer[USBIP_MAX_DATA_SIZE];
        // Pack the outgoing packets into a USB/IP packet format

        result = libusb_bulk_transfer(handle, USBIP_OUT_ENDPOINT, out_buffer, sizeof(out_buffer), &actual_length, 0);
        if (result < 0) {
            fprintf(stderr, "Error sending USB/IP packet: %s\n", libusb_error_name(result));
            break;
        }
    }

    libusb_release_interface(handle, 0);
    libusb_close(handle);
    libusb_exit(context);

    return NULL;
}


int usb_gadget_start(const char *gadgetfs_dir, libusb_device *device) {
    if (!gadgetfs_dir || !device) {
        return -1;
    }

    usb_device_info_t device_info;

    // Populate device_info from the libusb_device

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

    // Create a thread to handle USB/IP traffic
    pthread_t usbip_thread;
    thread_create_result = pthread_create(&usbip_thread, NULL, handle_usbip_traffic, device);
    if (thread_create_result != 0) {
        perror("Error creating USB/IP handling thread");
        close(gadgetfs_fd);
        return -1;
    }




    // Don't forget to join the GadgetFS event handling thread before exiting
    // pthread_join(gadgetfs_thread, NULL);

    return 0;
}
