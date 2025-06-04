#include "../include/create_dir.h"
#include "../typedef/standard_error.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int create_dir(const char *path, standard_error *out_err) {
  if (mkdir(path, 0755) == -1) {
    if (errno != EEXIST) { 
      if (out_err) {
        snprintf(out_err->message, ERR_MESSAGE_SIZE,
                 "failed to create directory %s: %s ", path, strerror(errno));
      }

      return ERR_MKDIR_FAILED;
    }
  }
  return ERR_NONE;
}
