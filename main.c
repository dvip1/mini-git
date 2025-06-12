#include "components/include/add.h"
#include "components/include/init.h"
#include "components/include/limits.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  char path[PATH_MAX];
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

  else if (strcmp(argv[1], "add") == 0) {
    const char **path1 = (const char **)argv;
    add(path1, argc, &err_out, path);
    return 0;
  }
}
