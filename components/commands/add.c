#include "../include/add.h"
#include "../include/create_blob.h"
#include "../include/is_exists.h"
#include "../include/limits.h"
#include "../include/read_file.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHA256_HEX_LENGTH 65

int add(const char *path[], int size, standard_error *out_err,
        const char *abs) {
  char mini_git_repo_path[PATH_MAX];
  int res = ERR_NONE;
  int final_result = ERR_NONE;
  char *file_path = NULL;
  char *content = NULL;
  snprintf(mini_git_repo_path, sizeof(mini_git_repo_path), "%s/.mini-git/",
           abs);
  if (is_directory_exist(mini_git_repo_path) == 0) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE,
             "mini-git directory isn't initiated.");
    out_err->code = ERR_REPOSITORY_NOT_FOUND;
    return out_err->code;
  }

  if (size < 3) {
    printf("Usage: mini-git add <file-name>\n");
    snprintf(out_err->message, ERR_MESSAGE_SIZE,
             "Usage: mini-git add <file-name>");
    out_err->code = ERR_COMMAND;
    return out_err->code;
  }

  for (int i = 2; i < size; i++) {
    size_t full_path_len = strlen(abs) + 1 + strlen(path[i]) + 1;
    file_path = malloc(full_path_len);
    snprintf(file_path, full_path_len, "%s/%s", abs, path[i]);
    if (!file_path) {
      snprintf(out_err->message, ERR_MESSAGE_SIZE,
               "Memory allocation failed: %s", strerror(errno));
      out_err->code = ERR_OUT_OF_MEMORY;
      goto cleanup;
    }

    if (!is_file_exist(file_path)) {
      snprintf(out_err->message, ERR_MESSAGE_SIZE, "File not found: %s",
               file_path);
      fprintf(stderr, "Warning: File doesn't Exists %s: %s\n", file_path,
              out_err->message);
      out_err->code = FILE_NOT_FOUND;
      continue;
    }

    content = read_file(file_path, out_err);
    if (!content) {
      fprintf(stderr, "Warning: Failed to read %s: %s\n", path[i],
              out_err->message);
      final_result = out_err->code;
      free(file_path);
      file_path = NULL;
      continue;
    }

    char hash_out[SHA256_HEX_LENGTH];
    res = create_blob(content, abs, hash_out, out_err);
    if (res != ERR_NONE) {
      fprintf(stderr, "Warning: Failed to create blob for %s: %s\n", path[i],
              out_err->message);
      final_result = out_err->code;
    }

    // free and reset for next iteration
    free(file_path);
    file_path = NULL;
    free(content);
    content = NULL;
  }

cleanup:
  if (file_path)
    free(file_path);
  if (content)
    free(content);

  return (final_result != 0) ? final_result : out_err->code;
}
