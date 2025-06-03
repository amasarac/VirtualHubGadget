//cli.h

#ifndef CLI_H
#define CLI_H

#include <stdbool.h>
#include "transfer.h"

typedef enum {
    CONNECTION_NONE,
    CONNECTION_IN_PROGRESS,
    CONNECTION_ESTABLISHED
} connection_state_t;

typedef struct {
    connection_state_t usbip_connection_state;
    bool usbip_connected;
} cli_t;


void cli_init(cli_t **cli_ptr);
void cli_connect(cli_t *cli, const char *server_address);
void cli_disconnect(cli_t *cli);
void cli_cleanup(cli_t *cli);

/* Additional CLI commands */
void cli_list_devices(void);
void cli_connect_device(void);
void cli_disconnect_device(void);
void cli_list_connected_devices(void);

#endif // CLI_H
