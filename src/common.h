#ifndef COMMON_H
#define COMMON_H

#include <stddef.h>
#include <stdint.h>

/* Basic data structure for interrupt transfers. */
typedef struct {
    int endpoint_fd;
    void *data;
    size_t length;
} interrupt_transfer_t;

/* Basic data structure for bulk transfers. */
typedef struct {
    int endpoint_fd;
    void *data;
    size_t length;
} bulk_transfer_t;

#endif /* COMMON_H */
