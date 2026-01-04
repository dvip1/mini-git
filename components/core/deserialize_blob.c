#include "../include/deserialize_blob.h"
#include <errno.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#define HASH_SIZE 65

int deserialize_blob(const char *repo_path, const char *hash,
                     char **out_content, standard_error *out_err) {
  if (!repo_path || !hash || !out_content) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "Null parameter");
    return ERR_INVALID_RESPONSE;
  }

  char path[1024];
  snprintf(path, sizeof(path), "%s/.mini-git/objects/%.2s/%s", repo_path, hash,
           hash + 2);

  FILE *fp = fopen(path, "rb");
  if (!fp) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "Failed to open blob: %s",
             strerror(errno));
    return ERR_READ_FAILED;
  }

  fseek(fp, 0, SEEK_END);
  long fsize = ftell(fp);
  rewind(fp);

  char *buffer = malloc(fsize + 1);
  if (!buffer) {
    fclose(fp);
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "Memory allocation failed");
    return ERR_OUT_OF_MEMORY;
  }

  fread(buffer, 1, fsize, fp);
  fclose(fp);
  buffer[fsize] = '\0';

  // Skip the header: find the first '\0'
  char *content_start = strchr(buffer, '\0');
  if (!content_start) {
    free(buffer);
    snprintf(out_err->message, ERR_MESSAGE_SIZE,
             "Malformed blob, no header delimiter");
    return ERR_INVALID_RESPONSE;
  }

  content_start++; // Move past '\0'
  size_t content_len = fsize - (content_start - buffer);

  char *content = malloc(content_len + 1);
  if (!content) {
    free(buffer);
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "Memory allocation failed");
    return ERR_OUT_OF_MEMORY;
  }

  memcpy(content, content_start, content_len);
  content[content_len] = '\0';

  free(buffer);
  *out_content = content;
  return ERR_NONE;
}
