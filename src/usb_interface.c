#include "usb_interface.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

usb_interface_t *usb_interface_create(uint8_t interface_number,
                                      uint8_t alternate_setting)
{
    usb_interface_t *interface = malloc(sizeof(*interface));
    if (!interface)
        return NULL;
    interface->interface_number = interface_number;
    interface->alternate_setting = alternate_setting;
    interface->interface_fd = -1;
    return interface;
}

void usb_interface_destroy(usb_interface_t *interface)
{
    if (!interface)
        return;
    if (interface->interface_fd >= 0)
        close(interface->interface_fd);
    free(interface);
}

bool usb_interface_claim(usb_interface_t *interface)
{
    if (!interface || interface->interface_fd >= 0)
        return false;

    char path[64];
    snprintf(path, sizeof(path), "/dev/bus/usb/%03d/%03d", 0,
             interface->interface_number);
    int fd = open(path, O_RDWR);
    if (fd < 0)
        return false;

    interface->interface_fd = fd;
    return true;
}

bool usb_interface_release(usb_interface_t *interface)
{
    if (!interface || interface->interface_fd < 0)
        return false;
    close(interface->interface_fd);
    interface->interface_fd = -1;
    return true;
}
