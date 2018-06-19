
#include "collectd.h"

#include "common.h"
#include "plugin.h"

#include "utils_threshold.h"
#include "utils_cmd_listthreshold.h"
#include "utils_parse_option.h"

cmd_status_t cmd_parse_listthreshold(size_t argc, char **argv,
                               const cmd_options_t *opts
                               __attribute__((unused)),
                               cmd_error_handler_t *err) {
  if (argc != 0) {
    cmd_error(CMD_PARSE_ERROR, err, "Garbage after end of command: `%s'.",
              argv[0]);
    return CMD_PARSE_ERROR;
  }

  return CMD_OK;
} /* cmd_status_t cmd_parse_listthreshold */

#define free_everything_and_return(status)                                     \
  do {                                                                         \
    for (size_t j = 0; j < number; j++) {                                      \
      sfree(names[j]);                                                         \
      names[j] = NULL;                                                         \
    }                                                                          \
    sfree(names);                                                              \
    return status;                                                             \
  } while (0)

#define print_to_socket(fh, ...)                                               \
  do {                                                                         \
    if (fprintf(fh, __VA_ARGS__) < 0) {                                        \
      WARNING("handle_listthreshold: failed to write to socket #%i: %s", fileno(fh), \
              STRERRNO);                                                       \
      free_everything_and_return(CMD_ERROR);                                   \
    }                                                                          \
    fflush(fh);                                                                \
  } while (0)

cmd_status_t cmd_handle_listthreshold(FILE *fh, char *buffer) {
  cmd_error_handler_t err = {cmd_error_fh, fh};
  cmd_status_t status;
  cmd_t cmd;

  char **names = NULL;
  size_t number = 0;

  DEBUG("utils_cmd_listthreshold: handle_listthreshold (fh = %p, buffer = %s);", (void *)fh,
        buffer);

  if ((status = cmd_parse(buffer, &cmd, NULL, &err)) != CMD_OK)
    return status;
  if (cmd.type != CMD_LISTTHRESHOLD) {
    cmd_error(CMD_UNKNOWN_COMMAND, &err, "Unexpected command: `%s'.",
              CMD_TO_STRING(cmd.type));
    free_everything_and_return(CMD_UNKNOWN_COMMAND);
  }

  status = ut_get_names(&names, &number);
  if (status != 0) {
    DEBUG("command listthreshold: ut_get_names failed with status %i", status);
    cmd_error(CMD_ERROR, &err, "ut_get_names failed.");
    free_everything_and_return(CMD_ERROR);
  }

  print_to_socket(fh, "%i Threshold%s found\n", (int)number,
                  (number == 1) ? "" : "s");
  for (size_t i = 0; i < number; i++)
    print_to_socket(fh, "%s\n", names[i]);

  free_everything_and_return(CMD_OK);
} /* cmd_status_t cmd_handle_listthreshold */
