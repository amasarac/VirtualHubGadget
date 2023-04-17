#include "usb_interface.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/usbdevice_fs.h>

#define USBFS_CONTROL_REQUEST_TYPE 0x21
#define USBFS_CONTROL_REQUEST      0x09

#define USBFS_IOCTL_RETRY 4

struct usb_interface {
    uint8_t interface_number;
    uint8_t alternate_setting;
    int interface_fd;
};

usb_interface_t* usb_interface_create(uint8_t interface_number, uint8_t alternate_setting) {
    usb_interface_t *interface = (usb_interface_t *)malloc(sizeof(usb_interface_t));
    if (interface == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for USB interface.\n");
        exit(EXIT_FAILURE);
    }

    interface->interface_number = interface_number;
    interface->alternate_setting = alternate_setting;
    interface->interface_fd = -1;

    return interface;
}

void usb_interface_destroy(usb_interface_t *interface) {
    if (interface != NULL) {
        if (interface->interface_fd >= 0) {
            close(interface->interface_fd);
        }
        free(interface);
    }
}

bool usb_interface_claim(usb_interface_t *interface) {
    if (interface == NULL) {
        return false;
    }

    char filename[64];
    sprintf(filename, "/dev/bus/usb/%03d/%03d", 0, interface->interface_number);

    int interface_fd = open(filename, O_RDWR);
    if (interface_fd < 0) {
        fprintf(stderr, "Error: Failed to open USB interface %d.\n", interface->interface_number);
        return false;
    }

    struct usbdevfs_ioctl command;
    command.ifno = interface->interface_number;
    command.ioctl_code = USBDEVFS_IOCTL_CLAIM_INTERFACE;
    command.data = &interface->alternate_setting;
    command.length = sizeof(interface->alternate_setting);
    command.flags = USBDEVFS_IOCTL_RETRIES | USBDEVFS_REAPURB;

    for (int i = 0; i < USBFS_IOCTL_RETRY; ++i) {
        if (ioctl(interface_fd, USBDEVFS_IOCTL, &command) < 0) {
            perror("ioctl");
            close(interface_fd);
            return false;
        }
        if (command.status == 0) {
            interface->interface_fd = interface_fd;
            return true;
        }
        usleep(1000);
    }

    close(interface_fd);
    return false;
}

bool usb_interface_release(usb_interface_t *interface) {
    if (interface->claimed) {
        if (usb_device_release_interface(interface->device->handle, interface->number) < 0) {
            perror("Error releasing interface");
            return false;
        }
        interface->claimed = false;
    }
    return true;
}

bool usb_interface_reset(usb_interface_t *interface) {
    if (!interface->claimed) {
        fprintf(stderr, "Error: Interface must be claimed before it can be reset.\n");
        return false;
    }
    if (usb_device_reset(interface->device) == -1) {
        perror("Error resetting device");
        return false;
    }
    return true;
}
