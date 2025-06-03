#ifndef USB_INTERFACE_H
#define USB_INTERFACE_H

#include <stdbool.h>
#include <stdint.h>
#include "usb_device.h"
#include <libusb-1.0/libusb.h>

typedef struct {
    libusb_device_handle *handle;
    uint8_t interface_number;
} usb_interface_t;

usb_device_t *usb_device_open(uint16_t vendor_id, uint16_t product_id);
void usb_device_close(usb_device_t *device);
bool usb_device_control_transfer(usb_device_t *device, uint8_t request_type, uint8_t request, uint16_t value, uint16_t index, uint8_t *data, uint16_t length, uint32_t timeout);
bool usb_device_bulk_transfer(usb_device_t *device, uint8_t endpoint_address, uint8_t *data,uint32_t length, uint32_t timeout);
bool usb_device_interrupt_transfer(usb_device_t *device, uint8_t endpoint_address, uint8_t *data,uint32_t length, uint32_t timeout);
bool usb_device_claim_interface(usb_device_t *device, uint8_t interface_number);
bool usb_device_release_interface(usb_device_t *device, uint8_t interface_number);
bool usb_device_reset(usb_device_t *device);
void usb_interface_destroy(usb_interface_t *interface);
bool usb_interface_claim(usb_interface_t *interface);
bool usb_interface_release(usb_interface_t *interface);

#endif /* USB_INTERFACE_H */
