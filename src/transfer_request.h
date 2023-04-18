#ifndef _TRANSFER_REQUEST_H
#define _TRANSFER_REQUEST_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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
    uint8_t endpoint;
    transfer_direction_t direction;
    transfer_type_t type;
    transfer_buffer_t data;
    bool short_packet;
} transfer_request_t;

#endif /* _TRANSFER_REQUEST_H */
