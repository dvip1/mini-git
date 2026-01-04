#ifndef CREATE_BLOB_H
#define CREATE_BLOB_H
#include "../typedef/standard_error.h"
#include <stdio.h>

int create_blob(const char *content, const char *repo_path, char *out_hash,
                standard_error *out_err);

int write_binary_file(const char *path, const char *content, size_t len,
                      standard_error *out_err);

#endif
