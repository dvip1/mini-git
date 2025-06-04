#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int write_file(const char *path, const char *content) {
  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    perror("open");
    return -1;
  }

  ssize_t bytes_written = write(fd, content, strlen(content));
  if (bytes_written == -1) {
    perror("write");
    close(fd);
    return -1;
  }

  close(fd);
  return 0;
}
