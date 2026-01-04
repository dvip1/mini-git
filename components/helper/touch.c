#include "../include/touch.h"
#include "../typedef/standard_error.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

int touch(const char *path, standard_error *out_err) {
  int fd = open(path, O_CREAT | O_WRONLY, 0644);
  if (fd == -1) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE,
             "failed to open or create file '%s': %s", path, strerror(errno));
    return ERR_CREATE_FILE;
  }
  close(fd);

  if (utime(path, NULL) == -1) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE,
             "failed to update timestamp for '%s': %s", path, strerror(errno));
    return ERR_CREATE_FILE;
  }

  return ERR_NONE;
}
