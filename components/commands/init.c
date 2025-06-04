#include "../include/init.h"
#include "../include/create_dir.h"
#include "../typedef/standard_error.h"
#include <stdio.h>
#include <string.h>

int init(const char *path, standard_error *out_err) {
  char gitDir[512];
  int res;
  snprintf(gitDir, sizeof(gitDir), "%s/.mini-git", path);
  if ((res = create_dir(gitDir, out_err)) != ERR_NONE)
    return res;

  snprintf(gitDir, sizeof(gitDir), "%s/.mini-git/objects", path);
  if ((res = create_dir(gitDir, out_err)) != ERR_NONE)
    return res;

  snprintf(gitDir, sizeof(gitDir), "%s/.mini-git/refs", path);
  if ((res = create_dir(gitDir, out_err)) != ERR_NONE)
    return res;

  snprintf(gitDir, sizeof(gitDir), "%s/.mini-git/refs/head", path);
  if ((res = create_dir(gitDir, out_err)) != ERR_NONE)
    return res;

  printf("Initialized empty Mini Git repository in %s/.mini-git\n", path);
  return ERR_NONE;
}
