
#ifndef UTILS_CMD_LISTTHRESHOLD_H
#define UTILS_CMD_LISTTHRESHOLD_H 1

#include <stdio.h>

#include "utils_cmds.h"

cmd_status_t cmd_parse_listthreshold(size_t argc, char **argv,
                               const cmd_options_t *opts,
                               cmd_error_handler_t *err);

cmd_status_t cmd_handle_listthreshold(FILE *fh, char *buffer);

#endif /* UTILS_CMD_LISTTHRESHOLD_H */
