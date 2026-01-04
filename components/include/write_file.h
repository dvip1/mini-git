#ifndef WRITE_FILE_H
#define WRITE_FILE_H
#include "../typedef/standard_error.h"

int write_file(const char *path, const char *content, standard_error *out_err);

#endif
