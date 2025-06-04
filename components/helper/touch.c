#include "../include/touch.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <utime.h>

int touch(const char *path) {
  int fd = open(path, O_CREAT | O_WRONLY, 0644);
  if (fd == -1) {
    perror("open");
    return -1;
  }
  close(fd);

  if (utime(path, NULL) == -1) {
    perror("utime");
    return -1;
  }
  return 0;
}
