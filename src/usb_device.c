#include "usb_device.h"
#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <stdio.h>

usb_device_t* usb_device_open(uint16_t vendor_id, uint16_t product_id) {
    if (libusb_init(NULL) < 0) {
        return NULL;
    }

    libusb_device_handle *handle = libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
    if (!handle) {
        libusb_exit(NULL);
        return NULL;
    }

    usb_device_t *dev = calloc(1, sizeof(usb_device_t));
    if (!dev) {
        libusb_close(handle);
        libusb_exit(NULL);
        return NULL;
    }
    dev->handle = handle;
    dev->vendor_id = vendor_id;
    dev->product_id = product_id;
    return dev;
}

void usb_device_close(usb_device_t *dev) {
    if (!dev) {
        return;
    }
    if (dev->handle) {
        libusb_close(dev->handle);
    }
    libusb_exit(NULL);
    free(dev);
}

bool usb_device_control_transfer(usb_device_t *dev, uint8_t bmRequestType, uint8_t bRequest,
                                 uint16_t wValue, uint16_t wIndex, unsigned char *data,
                                 uint16_t wLength, unsigned int timeout) {
    if (!dev || !dev->handle) {
        return false;
    }
    int r = libusb_control_transfer(dev->handle, bmRequestType, bRequest,
                                    wValue, wIndex, data, wLength, timeout);
    return r >= 0;
}

bool usb_device_bulk_transfer(usb_device_t *dev, unsigned char endpoint, unsigned char *data,
                              int length, int *transferred, unsigned int timeout) {
    if (!dev || !dev->handle) {
        return false;
    }
    int r = libusb_bulk_transfer(dev->handle, endpoint, data, length, transferred, timeout);
    return r == 0;
}

bool usb_device_interrupt_transfer(usb_device_t *dev, unsigned char endpoint, unsigned char *data,
                                   int length, int *transferred, unsigned int timeout) {
    if (!dev || !dev->handle) {
        return false;
    }
    int r = libusb_interrupt_transfer(dev->handle, endpoint, data, length, transferred, timeout);
    return r == 0;
}

bool usb_device_claim_interface(usb_device_t *dev, int interface_number) {
    if (!dev || !dev->handle) {
        return false;
    }
    return libusb_claim_interface(dev->handle, interface_number) == 0;
}

bool usb_device_release_interface(usb_device_t *dev, int interface_number) {
    if (!dev || !dev->handle) {
        return false;
    }
    return libusb_release_interface(dev->handle, interface_number) == 0;
}

bool usb_device_reset(usb_device_t *dev) {
    if (!dev || !dev->handle) {
        return false;
    }
    return libusb_reset_device(dev->handle) == 0;
}
