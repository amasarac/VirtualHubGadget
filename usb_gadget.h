//usbgadget.h

#ifndef USB_GADGET_H
#define USB_GADGET_H

#include <libusb-1.0/libusb.h>
#include "gadgetfs_api.h"

int usb_gadget_start(const char *gadgetfs_dir, libusb_device *device);

#endif // USB_GADGET_H
