#include "usb_device.h"
#include <libusb-1.0/libusb.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USB_ENDPOINT_IN     LIBUSB_ENDPOINT_IN
#define USB_ENDPOINT_OUT    LIBUSB_ENDPOINT_OUT

struct usb_device {
    libusb_device_handle *dev_handle;
};

usb_device_t *usb_device_open(int vid, int pid) {
    libusb_device **devs;
    libusb_device_handle *dev_handle;
    int r;

    // Initialize libusb
    r = libusb_init(NULL);
    if (r < 0) {
        fprintf(stderr, "Error: Failed to initialize libusb: %s\n", libusb_error_name(r));
        exit(EXIT_FAILURE);
    }

    // Get list of USB devices
    ssize_t cnt = libusb_get_device_list(NULL, &devs);
    if (cnt < 0) {
        fprintf(stderr, "Error: Failed to get list of USB devices: %s\n", libusb_error_name(cnt));
        exit(EXIT_FAILURE);
    }

    // Iterate through devices and find one with matching VID/PID
    for (ssize_t i = 0; i < cnt; i++) {
        libusb_device *dev = devs[i];
        struct libusb_device_descriptor desc;

        r = libusb_get_device_descriptor(dev, &desc);
        if (r < 0) {
            fprintf(stderr, "Error: Failed to get device descriptor: %s\n", libusb_error_name(r));
            continue;
        }

        if (desc.idVendor == vid && desc.idProduct == pid) {
            // Open the device
            r = libusb_open(dev, &dev_handle);
            if (r < 0) {
                fprintf(stderr, "Error: Failed to open USB device: %s\n", libusb_error_name(r));
                continue;
            }

            // Claim the interface
            r = libusb_claim_interface(dev_handle, 0);
            if (r < 0) {
                fprintf(stderr, "Error: Failed to claim interface: %s\n", libusb_error_name(r));
                libusb_close(dev_handle);
                continue;
            }

            // Free device list and return the device handle
            libusb_free_device_list(devs, 1);
            usb_device_t *dev_struct = malloc(sizeof(usb_device_t));
            dev_struct->dev_handle = dev_handle;
            return dev_struct;
        }
    }

    // Free device list and return NULL if no matching device found
    libusb_free_device_list(devs, 1);
    return NULL;
}

void usb_device_close(usb_device_t *device) {
    // Close device handle
    close(device->fd);

    // Free device memory
    free(device);
}

usb_interface_t* usb_device_get_interface_by_class(usb_device_t *device, uint8_t interface_class) {
    for (int i = 0; i < device->num_interfaces; i++) {
        if (device->interfaces[i]->interface_descriptor->bInterfaceClass == interface_class) {
            return device->interfaces[i];
        }
    }
    return NULL;
}

usb_interface_t* usb_device_get_interface_by_subclass(usb_device_t *device, uint8_t interface_subclass) {
    for (int i = 0; i < device->num_interfaces; i++) {
        if (device->interfaces[i]->interface_descriptor->bInterfaceSubClass == interface_subclass) {
            return device->interfaces[i];
        }
    }
    return NULL;
}

usb_interface_t* usb_device_get_interface_by_protocol(usb_device_t *device, uint8_t interface_protocol) {
    for (int i = 0; i < device->num_interfaces; i++) {
        if (device->interfaces[i]->interface_descriptor->bInterfaceProtocol == interface_protocol) {
            return device->interfaces[i];
        }
    }
    return NULL;
}

void usb_device_reset(usb_device_t *device) {
    int res = ioctl(device->fd, USBDEVFS_RESET, 0);
    if (res < 0) {
        perror("Error: Failed to reset USB device.");
    }
}

int usb_device_control_transfer(usb_device_handle_t *dev_handle, uint8_t bmRequestType, uint8_t bRequest,
                                uint16_t wValue, uint16_t wIndex, unsigned char *data, uint16_t wLength,
                                unsigned int timeout_ms) {
    int ret = libusb_control_transfer(dev_handle->handle, bmRequestType, bRequest, wValue, wIndex, data, wLength,
                                      timeout_ms);
    if (ret < 0) {
        fprintf(stderr, "Error: Failed to perform USB control transfer: %s.\n", libusb_error_name(ret));
        return -1;
    }

    return ret;
}

void usb_device_close(usb_device_handle_t *dev_handle) {
    if (dev_handle == NULL) {
        fprintf(stderr, "Error: Device handle is NULL.\n");
        return;
    }

    libusb_close(dev_handle->handle);
    free(dev_handle);
}

int usb_device_claim_interface(usb_device_handle_t *dev_handle, int interface_number) {
    int ret = libusb_claim_interface(dev_handle->handle, interface_number);
    if (ret < 0) {
        fprintf(stderr, "Error: Failed to claim interface %d: %s.\n", interface_number, libusb_error_name(ret));
        return -1;
    }

    return ret;
}

int usb_device_release_interface(usb_device_handle_t *dev_handle, int interface_number) {
    int ret = libusb_release_interface(dev_handle->handle, interface_number);
    if (ret < 0) {
        fprintf(stderr, "Error: Failed to release interface %d: %s.\n", interface_number, libusb_error_name(ret));
        return -1;
    }

    return ret;
}
