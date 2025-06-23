#include "../include/init.h"
#include "../include/create_dir.h"
#include "../include/touch.h"
#include "../include/write_file.h"
#include "../typedef/standard_error.h"
#include <stdio.h>

#define CREATE_DIR(subpath)                                                    \
  do {                                                                         \
    snprintf(gitDir, sizeof(gitDir), "%s/%s", path, subpath);                  \
    if ((res = create_dir(gitDir, out_err)) != ERR_NONE)                       \
      return res;                                                              \
  } while (0)

int init(const char *path, standard_error *out_err) {
  char gitDir[512];
  int res;

  CREATE_DIR(".mini-git");
  CREATE_DIR(".mini-git/objects");
  CREATE_DIR(".mini-git/refs");
  CREATE_DIR(".mini-git/refs/heads");

  snprintf(gitDir, sizeof(gitDir), "%s/.mini-git/HEAD", path);
  if ((res = touch(gitDir, out_err)) != ERR_NONE)
    return res;
  const char *masterContent = "ref: refs/heads/master\n";
  if ((res = write_file(gitDir, masterContent, out_err)) != ERR_NONE)
    return res;
  const char *configContent = "[core]\n"
                              "    repositoryformatversion = 0\n"
                              "    filemode = false\n"
                              "    bare = false\n";

  snprintf(gitDir, sizeof(gitDir), "%s/.mini-git/config", path);
  if ((res = write_file(gitDir, configContent, out_err)) != ERR_NONE)
    return res;
  printf("Initialized empty Mini Git repository in %s/.mini-git\n", path);
  return ERR_NONE;
}
