#include "usb_interface.h"
#include <stdlib.h>

void usb_interface_destroy(usb_interface_t *interface) {
    free(interface);
}

bool usb_interface_claim(usb_interface_t *interface) {
    if (!interface || !interface->handle) {
        return false;
    }
    return libusb_claim_interface(interface->handle, interface->interface_number) == 0;
}

bool usb_interface_release(usb_interface_t *interface) {
    if (!interface || !interface->handle) {
        return false;
    }
    return libusb_release_interface(interface->handle, interface->interface_number) == 0;
}
