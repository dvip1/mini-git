#include "../include/is_exists.h"
#include <sys/stat.h>

int is_directory_exist(const char *path) {
  struct stat st;
  return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

int is_file_exist(const char *path) {
  struct stat st;
  return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}
