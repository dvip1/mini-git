#ifndef DESERIALIZE_BLOB_H
#define DESERIALIZE_BLOB_H
#include "../typedef/standard_error.h"

int deserialize_blob(const char *repo_path, const char *hash,
                     char **out_content, standard_error *out_err);

#endif
