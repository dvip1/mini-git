#include "../include/create_blob.h"
#include "../include/create_dir.h"
#include "../include/touch.h"
#include "../include/write_file.h"
#include <errno.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#define HASH_SIZE 65
#include <unistd.h>

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
  if ((res = write_binary_file(file_path, blob_data, strlen(content),
                               out_err)) != ERR_NONE)
    goto cleanup;

  res = ERR_NONE;

cleanup:
  free(blob_data);
  return res;
}

int write_binary_file(const char *path, const char *content, size_t len,
                      standard_error *out_err) {
  int fd = open(path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  if (fd == -1) {
    snprintf(out_err->message, ERR_MESSAGE_SIZE,
             "failed to open or create file '%s': %s", path, strerror(errno));
    return ERR_CREATE_FILE;
  }

  ssize_t total_written = 0;

  while ((size_t)total_written < len) {
    ssize_t written = write(fd, content + total_written, len - total_written);
    if (written == -1) {
      close(fd);
      snprintf(out_err->message, ERR_MESSAGE_SIZE,
               "failed to write to file '%s': %s", path, strerror(errno));
      return ERR_WRITE_FAILED;
    }
    total_written += written;
  }

  close(fd);
  return ERR_NONE;
}
