#ifndef CREATE_BLOB_H
#define CREATE_BLOB_H
#include "../typedef/standard_error.h"

int create_blob(const char *content, const char *repo_path, char *out_hash,
                standard_error *out_err);

#endif
