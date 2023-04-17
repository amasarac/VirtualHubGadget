#ifndef TRANSFER_H
#define TRANSFER_H

#include <stddef.h>
#include <stdint.h>

typedef enum {
    TRANSFER_TYPE_CONTROL,
    TRANSFER_TYPE_ISOCHRONOUS,
    TRANSFER_TYPE_INTERRUPT,
    TRANSFER_TYPE_BULK
} transfer_type_t;

typedef struct {
    transfer_type_t type;
    uint8_t endpoint;
    void* data;
    uint32_t length;
    uint32_t rate; // Only used for isochronous transfers
} transfer_request_t;

typedef struct {
    transfer_type_t type;
    uint8_t endpoint;
    void* data;
    uint32_t length;
    uint32_t rate; // Only used for isochronous transfers
    int status;
} transfer_t;

#endif /* TRANSFER_H */
