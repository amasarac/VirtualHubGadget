#include "usb_device.h"
#include <libusb-1.0/libusb.h>
#include <stdlib.h>
#include <stdio.h>

usb_device_t *usb_device_open(uint16_t vendor_id, uint16_t product_id)
{
    if (libusb_init(NULL) < 0)
        return NULL;

    libusb_device_handle *handle =
        libusb_open_device_with_vid_pid(NULL, vendor_id, product_id);
    if (!handle)
        return NULL;

    usb_device_t *device = malloc(sizeof(*device));
    if (!device) {
        libusb_close(handle);
        return NULL;
    }

    device->handle = handle;
    device->vendor_id = vendor_id;
    device->product_id = product_id;
    return device;
}

void usb_device_close(usb_device_t *device)
{
    if (!device)
        return;
    if (device->handle)
        libusb_close(device->handle);
    libusb_exit(NULL);
    free(device);
}

bool usb_device_control_transfer(usb_device_t *device, uint8_t bmRequestType,
                                 uint8_t bRequest, uint16_t wValue,
                                 uint16_t wIndex, unsigned char *data,
                                 uint16_t wLength, unsigned int timeout)
{
    int r = libusb_control_transfer(device->handle, bmRequestType, bRequest,
                                    wValue, wIndex, data, wLength, timeout);
    return r >= 0;
}

bool usb_device_bulk_transfer(usb_device_t *device, unsigned char endpoint,
                              unsigned char *data, int length,
                              int *transferred, unsigned int timeout)
{
    int r = libusb_bulk_transfer(device->handle, endpoint, data, length,
                                  transferred, timeout);
    return r == 0;
}

bool usb_device_interrupt_transfer(usb_device_t *device, unsigned char endpoint,
                                   unsigned char *data, int length,
                                   int *transferred, unsigned int timeout)
{
    int r = libusb_interrupt_transfer(device->handle, endpoint, data, length,
                                       transferred, timeout);
    return r == 0;
}

bool usb_device_claim_interface(usb_device_t *device, int interface_number)
{
    return libusb_claim_interface(device->handle, interface_number) == 0;
}

bool usb_device_release_interface(usb_device_t *device, int interface_number)
{
    return libusb_release_interface(device->handle, interface_number) == 0;
}

bool usb_device_reset(usb_device_t *device)
{
    return libusb_reset_device(device->handle) == 0;
}
