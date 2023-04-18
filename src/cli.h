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
    // other members...
} cli_t;


void cli_init(cli_t *cli);
void cli_connect(cli_t *cli, const char *server_address);
void cli_disconnect(cli_t *cli);
void cli_cleanup(cli_t *cli);

#endif // CLI_H
