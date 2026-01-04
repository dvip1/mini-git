#include "../include/add.h"
#include "../include/create_blob.h"
#include "../include/dynamic_string_array.h"
#include "../include/gitignore_loader.h"
#include "../include/is_exists.h"
#include "../include/read_file.h"
#include <dirent.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#define SHA256_HEX_LENGTH 65
#define IGNORE_PATH_LIMIT PATH_MAX

int handle_file(const char *file_path, const char *abs,
                standard_error *out_err) {
  if (!is_file_exist(file_path)) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "File not found: %s",
             file_path);
    fprintf(stderr, "Warning: %s\n", out_err->message);
    out_err->code = ERR_FILE_NOT_FOUND;
    return out_err->code;
  }

  char *content = read_file(file_path, out_err);
  if (!content) {
    fprintf(stderr, "Warning: Failed to read %s: %s\n", file_path,
            out_err->message);
    return out_err->code;
  }

  char hash_out[SHA256_HEX_LENGTH];
  int res = create_blob(content, abs, hash_out, out_err);
  if (res != ERR_NONE) {
    fprintf(stderr, "Warning: Blob creation failed for %s: %s\n", file_path,
            out_err->message);
  }

  free(content);
  return res;
}

int handle_directory(const char *dir_path, const char *abs,
                     standard_error *out_err, int *index,
                     DynamicStringArray *arr) {
  DIR *dir = opendir(dir_path);
  if (!dir) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "Can't open directory: %s",
             dir_path);
    out_err->code = ERR_NOT_A_DIRECTORY;
    return out_err->code;
  }

  struct dirent *entry;
  struct stat path_stat;
  int result = ERR_NONE;

  while ((entry = readdir(dir)) != NULL) {
    if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
      continue;

    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

    if (stat(full_path, &path_stat) == -1)
      continue;

    const char *rel = full_path + strlen(abs) + 1;

    if (is_ignored(rel)) {
      continue;
    } else {
      insert_string(arr, arr->size, rel, out_err);
      index++;
    }

    if (S_ISDIR(path_stat.st_mode)) {
      handle_directory(full_path, abs, out_err, index, arr);
    } else if (S_ISREG(path_stat.st_mode)) {
      handle_directory(full_path, abs, out_err, index, arr);
      int res = handle_file(full_path, abs, out_err);
      if (res != ERR_NONE)
        result = res;
    }
  }

  closedir(dir);
  return result;
}

// ----------- Main Add -----------

int add(const char *path[], int size, standard_error *out_err,
        const char *abs) {
  char repo_path[PATH_MAX];
  snprintf(repo_path, sizeof(repo_path), "%s/.mini-git/", abs);
  DynamicStringArray *arr = create_array(out_err);
  int index = 0;
  if (!arr) {
    return out_err->code;
  }
  if (is_directory_exist(repo_path) == 0) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "mini-git not initialized");
    return (out_err->code = ERR_REPOSITORY_NOT_FOUND);
  }

  if (size < 3) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE,
             "Usage: mini-git add <file|dir|.>");
    printf("%s\n", out_err->message);
    return (out_err->code = ERR_COMMAND);
  }

  load_gitignore(abs);

  int final_result = ERR_NONE;
  // Special case: "add ."
  if (size == 3 && strcmp(path[2], ".") == 0) {
    final_result = handle_directory(abs, abs, out_err, &index, arr);
    goto cleanup;
  }

  for (int i = 2; i < size; i++) {
    char full_path[PATH_MAX];
    snprintf(full_path, sizeof(full_path), "%s/%s", abs, path[i]);

    struct stat path_stat;
    if (stat(full_path, &path_stat) == -1) {
      fprintf(stderr, "Warning: Path does not exist %s\n", full_path);
      continue;
    }

    if (is_ignored(path[i])) {
      continue;
    } else {
      insert_string(arr, arr->size, path[i], out_err);
      index++;
    }
    int res = S_ISDIR(path_stat.st_mode)
                  ? handle_directory(full_path, abs, out_err, &index, arr)
                  : handle_file(full_path, abs, out_err);

    if (res != ERR_NONE)
      final_result = res;
  }

  goto cleanup;

cleanup:
  destroy_array(arr);
  free_gitignore_patterns();
  return final_result;
}
