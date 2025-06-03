#ifndef USB_DEVICE_H
#define USB_DEVICE_H

#include <stdbool.h>
#include <stdint.h>
#include <libusb-1.0/libusb.h>

typedef struct {
    libusb_device_handle *handle;
    uint16_t vendor_id;
    uint16_t product_id;
} usb_device_t;

usb_device_t *usb_device_open(uint16_t vendor_id, uint16_t product_id);
void usb_device_close(usb_device_t *device);
bool usb_device_control_transfer(usb_device_t *device, uint8_t bmRequestType,
                                 uint8_t bRequest, uint16_t wValue,
                                 uint16_t wIndex, unsigned char *data,
                                 uint16_t wLength, unsigned int timeout);
bool usb_device_bulk_transfer(usb_device_t *device, unsigned char endpoint,
                              unsigned char *data, int length,
                              int *transferred, unsigned int timeout);
bool usb_device_interrupt_transfer(usb_device_t *device, unsigned char endpoint,
                                   unsigned char *data, int length,
                                   int *transferred, unsigned int timeout);
bool usb_device_claim_interface(usb_device_t *device, int interface_number);
bool usb_device_release_interface(usb_device_t *device, int interface_number);
bool usb_device_reset(usb_device_t *device);

#endif /* USB_DEVICE_H */
