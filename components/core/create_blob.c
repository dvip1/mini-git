#include "../include/create_blob.h"
#include "../include/create_dir.h"
#include "../include/touch.h"
#include "../include/write_file.h"
#include <errno.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#define HASH_SIZE 65

int create_blob(const char *content, const char *repo_path, char *out_hash,
                standard_error *out_err) {
  // check everything
  if (!content || !repo_path || !out_hash) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "Null parameter");
    return ERR_INVALID_RESPONSE;
  }

  // initialize
  size_t content_len = strlen(content);
  char header[64];
  snprintf(header, sizeof(header), "blob %zu", content_len);

  size_t blob_len = strlen(header) + 1 + content_len;
  char *blob_data = malloc(blob_len);
  if (!blob_data) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "Memory allocation failed: %s",
             strerror(errno));
    return ERR_OUT_OF_MEMORY;
  }

  // initialize blob_data
  memcpy(blob_data, header, strlen(header));
  blob_data[strlen(header)] = '\0';
  memcpy(blob_data + strlen(header) + 1, content, content_len);

  // 🧠 Hash the blob
  unsigned char hash[SHA256_DIGEST_LENGTH];
  SHA256((unsigned char *)blob_data, blob_len, hash);

  for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i)
    sprintf(out_hash + i * 2, "%02x", hash[i]);
  out_hash[64] = '\0';

  // 📁 Create paths
  char dir_path[512], file_path[1024];
  snprintf(dir_path, sizeof(dir_path), "%s/.mini-git/objects/%.2s", repo_path,
           out_hash);
  snprintf(file_path, sizeof(file_path), "%s/%s", dir_path, out_hash + 2);

  // do the write up
  int res;
  if ((res = create_dir(dir_path, out_err)) != ERR_NONE)
    goto cleanup;
  if ((res = touch(file_path, out_err)) != ERR_NONE)
    goto cleanup;
  if ((res = write_file(file_path, blob_data, out_err)) != ERR_NONE)
    goto cleanup;

  res = ERR_NONE;

cleanup:
  free(blob_data);
  return res;
}
