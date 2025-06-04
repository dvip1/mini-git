#include "components/include/init.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  char path[256];
  standard_error err_out = {0};

  if (getcwd(path, sizeof(path)) == NULL) {
    printf("error in getcwd()\n");
    return 1;
  }

  if (argc < 2) {
    printf("Usage: gitemini <arg1> [<args>]\n");
    return 1;
  }

  if (strcmp(argv[1], "init") == 0) {
    int res = init(path, &err_out);
    if (res != 0) {
      fprintf(stderr, "Error: %s\n", err_out.message);
    }
    return res;
  }

  if (argc == 3) {
    printf("%s \t %s\n", argv[1], argv[2]);
  }
  printf("%s\n", argv[1]);
  return 0;
}
