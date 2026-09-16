#ifndef TRANSFER_H
#define TRANSFER_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    TRANSFER_TYPE_CONTROL = 0,
    TRANSFER_TYPE_BULK,
    TRANSFER_TYPE_INTERRUPT,
    TRANSFER_TYPE_ISOCHRONOUS
} transfer_type_t;

typedef enum {
    TRANSFER_DIRECTION_OUT = 0,
    TRANSFER_DIRECTION_IN
} transfer_direction_t;

typedef struct {
    uint8_t *buffer;
    size_t length;
} transfer_buffer_t;

typedef struct {
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint16_t wValue;
    uint16_t wIndex;
    uint16_t wLength;
} setup_packet_t;

typedef struct {
    setup_packet_t setup_packet;
    transfer_buffer_t data;
} control_transfer_t;

typedef struct {
    transfer_type_t type;
    uint8_t endpoint;
    transfer_direction_t direction;
    void *data;
    uint32_t length;
    uint32_t rate; // Only used for isochronous transfers
    bool short_packet;
    int status;
} transfer_request_t;

typedef struct {
    int endpoint_fd;
    uint8_t endpoint;
    void *data;
    uint32_t length;
} interrupt_transfer_t;

typedef struct {
    int endpoint_fd;
    uint8_t endpoint;
    void *data;
    uint32_t length;
} bulk_transfer_t;

typedef transfer_request_t transfer_t;

#endif /* TRANSFER_H */
