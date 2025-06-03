//cli.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "cli.h"


void cli_init(cli_t **cli_ptr) {
    *cli_ptr = (cli_t*) malloc(sizeof(cli_t));
    if (*cli_ptr == NULL) {
        fprintf(stderr, "Error: unable to allocate memory for cli.\n");
        exit(EXIT_FAILURE);
    }
    memset(*cli_ptr, 0, sizeof(cli_t));
}


void cli_connect(cli_t *cli, const char *server_address) {
    if (cli->usbip_connected) {
        printf("USB/IP connection is already established.\n");
        return;
    }

    // Construct the `usbip` command to list devices exported by the server
char cmd_list[1024];
snprintf(cmd_list, sizeof(cmd_list), "usbip --debug list -r %s", server_address);

// Execute the command and capture its output
FILE *fp = popen(cmd_list, "r");
if (!fp) {
    fprintf(stderr, "Error listing devices exported by USB/IP server: %s\n", strerror(errno));
    return;
}

// Print out the list of devices
printf("Devices exported by USB/IP server at %s:\n", server_address);
char line[1024];
while (fgets(line, sizeof(line), fp)) {
    printf("%s", line);
}
printf("\n");

// Ask for user input to select devices
char input[1024];
printf("Enter the IDs of the devices to connect to (comma-separated): ");
if (!fgets(input, sizeof(input), stdin)) {
    fprintf(stderr, "Error reading user input: %s\n", strerror(errno));
    return;
}

// Construct the `usbip` command to connect to the selected devices
char cmd_connect[1024];
snprintf(cmd_connect, sizeof(cmd_connect), "usbip --debug attach -r %s %s", server_address, input);

// Execute the command and check for errors
int ret = system(cmd_connect);
if (ret != 0) {
    fprintf(stderr, "Error connecting to USB/IP server: %s\n", strerror(errno));
    return;
}

cli->usbip_connected = true;
printf("USB/IP connection established with server at %s\n", server_address);

    

    cli->usbip_connected = true;
    printf("USB/IP connection established with server at %s\n", server_address);
}

void cli_disconnect(cli_t *cli) {
    if (!cli->usbip_connected) {
        printf("USB/IP connection is not established.\n");
        return;
    }

    // List the USB devices that are currently attached
    printf("USB devices currently attached:\n");
    system("usbip list -r localhost");

    // Ask the user which devices to disconnect
    printf("Enter the IDs of the devices to disconnect (comma-separated): ");
    char ids[1024];
    fgets(ids, sizeof(ids), stdin);

    // Construct the `usbip` command to unbind and detach the selected devices
    char cmd[1024];
    snprintf(cmd, sizeof(cmd), "usbip unbind --usbids %s && usbip detach --usbids %s", ids, ids);

    // Execute the command and check for errors
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "Error disconnecting from USB/IP server: %s\n", strerror(errno));
        return;
    }

    cli->usbip_connected = false;
    printf("USB/IP connection terminated.\n");
}


void cli_cleanup(cli_t *cli) {
    if (cli->usbip_connected) {
        cli_disconnect(cli);
    }
}
