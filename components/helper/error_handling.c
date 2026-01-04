#include "../include/error_handling.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

void set_error(standard_error *err, int code, const char *fmt, ...) {
  if (!err)
    return;

  err->code = code;

  va_list args;
  va_start(args, fmt);
  vsnprintf(err->message, ERR_MESSAGE_SIZE, fmt, args);
  va_end(args);

  err->message[ERR_MESSAGE_SIZE - 1] = '\0';
}

void clear_error(standard_error *err) {
  if (!err)
    return;

  err->code = ERR_NONE;
  err->message[0] = '\0';
}

void set_error_with_warning(standard_error *err, int code, const char *fmt,
                            ...) {
  if (!err)
    return;

  err->code = code;

  va_list args;
  va_start(args, fmt);
  vsnprintf(err->message, ERR_MESSAGE_SIZE, fmt, args);
  va_end(args);

  err->message[ERR_MESSAGE_SIZE - 1] = '\0';

  // Print warning to stderr
  fprintf(stderr, "Warning: %s\nExit Code: %d\n", err->message, code);
}

void set_error_and_exit(standard_error *err, int code, const char *fmt, ...) {
  if (err) {
    err->code = code;

    va_list args;
    va_start(args, fmt);
    vsnprintf(err->message, ERR_MESSAGE_SIZE, fmt, args);
    va_end(args);

    err->message[ERR_MESSAGE_SIZE - 1] = '\0';

    fprintf(stderr, "Fatal Error: %s\nExit Code: %d\n", err->message, code);
  } else {
    fprintf(stderr, "Fatal Error (no error struct): code %d\n", code);
  }

  exit(code);
}
