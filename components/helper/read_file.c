#include "../include/read_file.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read_file(const char *filepath, standard_error *out_err) {
  FILE *file = fopen(filepath, "r");
  if (file == NULL) {
    fprintf(stderr, "Error: Could not open file '%s'\n", filepath);
    out_err->code = ERR_FILE_NOT_FOUND;
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "failed to find file  %s: %s ",
             filepath, strerror(errno));
    return NULL;
  }

  // Get file size
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  // Allocate memory for file content + null terminator
  char *content = malloc(file_size + 1);
  if (content == NULL) {
    fprintf(stderr, "Error: Memory allocation failed\n");
    fclose(file);
    out_err->code = ERR_READ_FAILED;
    snprintf(out_err->message, ERR_MESSAGE_SIZE, "failed to read file  %s: %s ",
             filepath, strerror(errno));
    return NULL;
  }

  // Read file content
  size_t bytes_read = fread(content, 1, file_size, file);
  content[bytes_read] = '\0';

  fclose(file);
  snprintf(out_err->message, ERR_MESSAGE_SIZE, "failed to find file  %s: %s ",
           filepath, strerror(errno));

  return content;
}
