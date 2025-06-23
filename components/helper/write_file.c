#include "../include/write_file.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int write_file(const char *path, const char *content, standard_error *out_err) {
  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE,
             "failed to open or create file '%s': %s", path, strerror(errno));
    return ERR_CREATE_FILE;
  }

  size_t len = strlen(content);
  ssize_t total_written = 0;

  while ((size_t)total_written < len) {
    ssize_t written = write(fd, content + total_written, len - total_written);
    if (written == -1) {
      close(fd);
      snprintf(out_err->message, ERR_MESSAGE_SIZE,
               "failed to write to file '%s': %s", path, strerror(errno));
      return ERR_WRITE_FAILED;
    }
    total_written += written;
  }

  close(fd);
  return ERR_NONE;
}
