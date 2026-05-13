//gadgetfs_api.c

#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include "gadgetfs_api.h"
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <poll.h>

// Helper function: read a string descriptor from the device
static int read_string_descriptor(libusb_device_handle *dev_handle, uint8_t index, char *buf, size_t buf_len) {
    if (buf_len < 2) {
        return LIBUSB_ERROR_OVERFLOW;
    }

    int r = libusb_get_string_descriptor_ascii(dev_handle, index, (unsigned char *)buf, buf_len);
    if (r < 0) {
        return r;
    }

    if (buf[0] != buf_len || buf[1] != LIBUSB_DT_STRING) {
        return LIBUSB_ERROR_IO;
    }

    return r;
}


int gadgetfs_init(gadgetfs_t *gfs, const char *gadgetfs_dir, const usb_device_info_t *device_info) {
    if (!gfs || !gadgetfs_dir || !device_info) {
        return -1;
    }

    gfs->fd = open(gadgetfs_dir, O_RDWR);
    if (gfs->fd < 0) {
        perror("Error opening GadgetFS directory");
        return -1;
    }

    // Write the device descriptor
    int ret = write(gfs->fd, &device_info->device_desc, sizeof(device_info->device_desc));
    if (ret < 0) {
        perror("Error writing GadgetFS device descriptor");
        close(gfs->fd);
        return -1;
    }

    // Write the configuration descriptor
    ret = write(gfs->fd, &device_info->config_desc, sizeof(device_info->config_desc));
    if (ret < 0) {
        perror("Error writing GadgetFS configuration descriptor");
        close(gfs->fd);
        return -1;
    }

    // Set up the GadgetFS interface and endpoint descriptors
    // based on the devices discovered using libusb and received by
    // the Raspberry Pi over USB/IP
    for (uint8_t j = 0; j < device_info->config_desc.config.bNumInterfaces; j++) {
        const struct usb_interface_descriptor *iface = &device_info->config_desc.iface;
        for (uint8_t l = 0; l < iface->bNumEndpoints; l++) {
            const struct usb_endpoint_descriptor *ep_desc = &device_info->config_desc.ep;

            // Write the GadgetFS endpoint descriptor to the gadgetfs_fd
            ret = write(gfs->fd, ep_desc, sizeof(*ep_desc));
            if (ret < 0) {
                perror("Error writing GadgetFS endpoint descriptor");
                close(gfs->fd);
                return -1;
            }
        }
    }

    gfs->initialized = 1;
    return 0;
}

int gadgetfs_poll_event(int fd, struct usb_gadgetfs_event *event) {
    struct pollfd pfd = {
        .fd = fd,
        .events = POLLIN,
        .revents = 0
    };

    int ret = poll(&pfd, 1, 500);
    if (ret < 0) {
        if (errno == EINTR) {
            return 0;
        }
        perror("Error polling GadgetFS");
        return -1;
    } else if (ret > 0) {
        if (pfd.revents & POLLIN) {
            ssize_t len = read(fd, event, sizeof(struct usb_gadgetfs_event));
            if (len < 0) {
                perror("Error reading GadgetFS event");
                return -1;
            }
            return 1;
        }
    }

    return 0;
}

int gadgetfs_poll_fd(int fd) {
    struct pollfd pfd = {
        .fd = fd,
        .events = POLLIN,
    };

    int ret = poll(&pfd, 1, -1);
    if (ret < 0) {
        perror("Error polling GadgetFS file descriptor");
        return -1;
    }

    if (pfd.revents & POLLIN) {
        return 1;
    }

    return 0;
}

void *handle_device(void *arg) {
    int gadgetfs_fd = *(int *)arg;
    isochronous_transfer_queue_t iso_queue;

    isochronous_transfer_queue_init(&iso_queue, 8);

    while (1) {
        int poll_result = gadgetfs_poll_fd(gadgetfs_fd);
        if (poll_result == 1) {
            struct usb_gadgetfs_event event;
            if (gadgetfs_read_event(gadgetfs_fd, &event) == 0) {
                if (event.type == GADGETFS_SETUP) {
                    /* handle setup events if needed */
                }
            }
        } else if (poll_result == -1) {
            break;
        }
    }

    isochronous_transfer_queue_destroy(&iso_queue);

    return NULL;
}
int gadgetfs_handle_event(gadgetfs_t *gfs, usb_transfer_t *transfer) {
    if (!gfs || !gfs->initialized || !transfer) {
        fprintf(stderr, "GadgetFS not initialized\n");
        return -1;
    }

    ssize_t ret = -1;
    switch (transfer->transfer_type) {
        case CONTROL_TRANSFER:
        case BULK_TRANSFER:
        case INTERRUPT_TRANSFER:
        case ISOCHRONOUS_TRANSFER:
            ret = write(gfs->fd, transfer->data, transfer->length);
            break;
        default:
            fprintf(stderr, "Invalid transfer type: %d\n", transfer->transfer_type);
            return -1;
    }

    if (ret < 0) {
        perror("gadgetfs write failed");
        return -1;
    }

    return 0;
}

void gadgetfs_exit(gadgetfs_t *gfs) {
    if (gfs && gfs->fd >= 0) {
        close(gfs->fd);
        gfs->fd = -1;
        gfs->initialized = 0;
    }
}

int gadgetfs_read_event(int fd, struct usb_gadgetfs_event *event) {
    ssize_t len = read(fd, event, sizeof(*event));
    if (len < 0) {
        return -1;
    }
    return 0;
}

int gadgetfs_event(int fd, struct usb_gadgetfs_event *event) {
    struct usb_gadgetfs_event ev;
    struct usb_gadgetfs_event *out = event ? event : &ev;
    if (gadgetfs_read_event(fd, out) < 0)
        return -1;
    return out->type;
}