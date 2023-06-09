#ifndef GADGETFS_API_H
#define GADGETFS_API_H

#include <stdint.h>
#include <usr/include/linux/usb/ch9.h>
//gadgetfs_api.h

#include <usr/include/linux/usb/gadget.h>
#include <libusb-1.0/libusb.h>
#include <unistd.h>
#include "isochronous_queue.h"

struct usb_hub_config_descriptor {
    struct usb_config_descriptor config;
    struct usb_interface_descriptor iface;
    struct usb_endpoint_descriptor ep;
} __attribute__((packed));

typedef struct {
    struct usb_device_descriptor device_desc;
    struct usb_hub_config_descriptor config_desc;
} usb_device_info_t;

ssize_t cnt = libusb_get_device_list(ctx, &devs);
usb_device_info_t *devices = malloc(cnt * sizeof(usb_device_info_t));

for (ssize_t i = 0; i < cnt; i++) {
    libusb_device *device = devs[i];
    libusb_get_device_descriptor(device, &devices[i].dev_desc);
    libusb_get_active_config_descriptor(device, &devices[i].config_desc);
}

int gadgetfs_init(const char *gadgetfs_dir, const usb_device_info_t *device_info);
int gadgetfs_poll_fd(int fd);
void *handle_device(void *arg);

int gadgetfs_init(const char *mount_path);
int gadgetfs_write_descriptor(int gadgetfs_fd, const void *descriptor, uint16_t length);

int initialize_device_with_libusb(libusb_device *device, int *control_fd, int *endpoint_fd);
void handle_control_event(int control_fd, libusb_device_handle *dev_handle);
void handle_endpoint_event(int endpoint_fd, libusb_device_handle *dev_handle, isochronous_transfer_queue_t *isochronous_queue);

#endif // GADGETFS_API_H
