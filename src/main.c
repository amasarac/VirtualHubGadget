// Simplified main program
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cli.h"

int main(int argc, char **argv)
{
    cli_t *cli;
    cli_init(&cli);

    if (argc > 2 && strcmp(argv[1], "connect") == 0) {
        cli_connect(cli, argv[2]);
    } else if (argc > 1 && strcmp(argv[1], "disconnect") == 0) {
        cli_disconnect(cli);
    } else {
        printf("Usage: %s connect <server> | disconnect\n", argv[0]);
    }

    cli_cleanup(cli);
    free(cli);
    return 0;
}
