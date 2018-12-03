#include "collectd.h"

#include "common.h"
#include "plugin.h"
#include "utils_cmd_flushstate.h"
#include "daemon/utils_cache.h"

cmd_status_t cmd_parse_flushstate(size_t argc, char **argv, cmd_flushstate_t *ret_flushstate,
                             const cmd_options_t *opts,
                             cmd_error_handler_t *err) {
  char *identifier_copy;

  if ((ret_flushstate == NULL) || (opts == NULL)) {
    errno = EINVAL;
    cmd_error(CMD_ERROR, err, "Invalid arguments to cmd_parse_flushstate.");
    return CMD_ERROR;
  }

  if (argc != 1) {
    if (argc == 0)
      cmd_error(CMD_PARSE_ERROR, err, "Missing identifier.");
    else
      cmd_error(CMD_PARSE_ERROR, err, "Garbage after identifier: `%s'.",
                argv[1]);
    return CMD_PARSE_ERROR;
  }

  identifier_copy = sstrdup(argv[0]); 

  ret_flushstate->raw_identifier = identifier_copy;
  return CMD_OK;
} /* cmd_status_t cmd_parse_flushstate */

cmd_status_t cmd_handle_flushstate(FILE *fh, char *buffer) {
  cmd_error_handler_t err = {cmd_error_fh, fh};
  cmd_status_t status;
  cmd_t cmd;

  if ((fh == NULL) || (buffer == NULL))
    return -1;

  DEBUG("utils_cmd_flushstate: cmd_handle_flushstate (fh = %p, buffer = %s);", (void *)fh,
        buffer);

  if ((status = cmd_parse(buffer, &cmd, NULL, &err)) != CMD_OK)
    return status;
  if (cmd.type != CMD_FLUSHSTATE) {
    cmd_error(CMD_UNKNOWN_COMMAND, &err, "Unexpected command: `%s'.",
              CMD_TO_STRING(cmd.type));
    cmd_destroy(&cmd);
    return CMD_UNKNOWN_COMMAND;
  }

  if (uc_set_state_by_name(cmd.cmd.flushstate.raw_identifier, STATE_UNKNOWN) < 0 ) {
    DEBUG("cmd_handle_getval: plugin_get_ds (%s) == NULL;", cmd.cmd.getval.identifier.type);
    cmd_error(CMD_ERROR, &err, "Type `%s' is unknown.\n", cmd.cmd.getval.identifier.type);
    cmd_destroy(&cmd);
    return CMD_ERROR;
  }

  cmd_error(CMD_OK, &err, "Done");
  cmd_destroy(&cmd);
  return CMD_OK;
} /* cmd_status_t cmd_handle_flushstate */

void cmd_destroy_flushstate(cmd_flushstate_t *flushstate) {
  if (flushstate == NULL)
    return;

  sfree(flushstate->raw_identifier);
} /* void cmd_destroy_flushstate */
