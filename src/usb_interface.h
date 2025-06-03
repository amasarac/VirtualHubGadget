#ifndef USB_INTERFACE_H
#define USB_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    uint8_t interface_number;
    uint8_t alternate_setting;
    int interface_fd;
} usb_interface_t;

usb_interface_t *usb_interface_create(uint8_t interface_number,
                                      uint8_t alternate_setting);
void usb_interface_destroy(usb_interface_t *interface);
bool usb_interface_claim(usb_interface_t *interface);
bool usb_interface_release(usb_interface_t *interface);

#endif /* USB_INTERFACE_H */
