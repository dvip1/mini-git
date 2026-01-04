#include "../include/dynamic_string_array.h"
#include "../include/error_handling.h"
#include <stdlib.h>
#include <string.h>

static bool resize_array(DynamicStringArray *arr, size_t new_capacity,
                         standard_error *err) {
  if (!arr) {
    set_error_with_warning(err, ERR_NULL_POINTER,
                           "ERROR: NULL pointer passed to resize_array\n");
    return false;
  }

  // Check if we're exceeding maximum array size
  if (new_capacity > MAX_ARRAY_SIZE) {
    set_error_with_warning(err, ERR_ARRAY_FULL,
                           "ERROR: Array would exceed maximum size limit\n");
    return false;
  }

  char **new_strings = realloc(arr->strings, new_capacity * sizeof(char *));
  if (!new_strings) {
    set_error_with_warning(err, ERR_OUT_OF_MEMORY,
                           "ERROR: Failed to resize array - out of memory\n");
    return false;
  }

  arr->strings = new_strings;
  arr->capacity = new_capacity;
  return true;
}

DynamicStringArray *create_array(standard_error *err) {
  DynamicStringArray *arr = malloc(sizeof(DynamicStringArray));
  if (!arr) {
    set_error_with_warning(err, ERR_NULL_POINTER,
                           "ERROR: NULL POINTER ERROR. Couldn't allocate "
                           "memory for array structure\n");
    return NULL;
  }

  arr->capacity = INITIAL_CAPACITY;
  arr->size = 0;
  arr->strings = malloc(arr->capacity * sizeof(char *));

  if (!arr->strings) {
    free(arr);
    set_error_and_exit(err, ERR_OUT_OF_MEMORY,
                       "ERROR: OUT OF MEMORY. Failed to allocate strings "
                       "array\nNOW_EXITING..\n");
    return NULL;
  }

  return arr;
}

void destroy_array(DynamicStringArray *arr) {
  if (!arr)
    return;

  // Free all individual strings
  for (size_t i = 0; i < arr->size; i++) {
    if (arr->strings[i]) {
      free(arr->strings[i]);
    }
  }

  // Free the array of pointers
  free(arr->strings);

  // Free the structure itself
  free(arr);
}

bool add_string(DynamicStringArray *arr, const char *str, standard_error *err) {
  if (!arr) {
    set_error_with_warning(err, ERR_NULL_POINTER,
                           "ERROR: NULL array pointer in add_string\n");
    return false;
  }

  if (!str) {
    set_error_with_warning(err, ERR_INVALID_ARGS,
                           "ERROR: NULL string pointer in add_string\n");
    return false;
  }

  // Check string length
  size_t str_len = strlen(str);
  if (str_len >= MAX_STRING_LENGTH) {
    set_error_with_warning(err, ERR_STRING_TOO_LONG,
                           "ERROR: String exceeds maximum length limit\n");
    return false;
  }

  // Check if we're at capacity and would exceed max size
  if (arr->size >= MAX_ARRAY_SIZE) {
    set_error_with_warning(err, ERR_ARRAY_FULL,
                           "ERROR: Array has reached maximum size limit\n");
    return false;
  }

  // Resize if needed
  if (arr->size >= arr->capacity) {
    size_t new_capacity = arr->capacity * GROWTH_FACTOR;
    if (!resize_array(arr, new_capacity, err)) {
      return false;
    }
  }

  // Allocate memory for the new string (+1 for null terminator)
  arr->strings[arr->size] = malloc((str_len + 1) * sizeof(char));
  if (!arr->strings[arr->size]) {
    set_error_with_warning(err, ERR_OUT_OF_MEMORY,
                           "ERROR: Failed to allocate memory for string\n");
    return false;
  }

  // Use safe string copy
  int result = snprintf(arr->strings[arr->size], str_len + 1, "%s", str);
  if (result < 0 || (size_t)result >= str_len + 1) {
    free(arr->strings[arr->size]);
    set_error_with_warning(err, ERR_GENERIC,
                           "ERROR: Failed to copy string safely\n");
    return false;
  }

  arr->size++;
  return true;
}

bool insert_string(DynamicStringArray *arr, size_t index, const char *str,
                   standard_error *err) {
  if (!arr) {
    set_error_with_warning(err, ERR_NULL_POINTER,
                           "ERROR: NULL array pointer in insert_string\n");
    return false;
  }

  if (!str) {
    set_error_with_warning(err, ERR_INVALID_ARGS,
                           "ERROR: NULL string pointer in insert_string\n");
    return false;
  }

  if (index > arr->size) {
    set_error_with_warning(err, ERR_INDEX_OUT_OF_BOUNDS,
                           "ERROR: Index out of bounds in insert_string\n");
    return false;
  }

  // Check string length
  size_t str_len = strlen(str);
  if (str_len >= MAX_STRING_LENGTH) {
    set_error_with_warning(err, ERR_STRING_TOO_LONG,
                           "ERROR: String exceeds maximum length limit\n");
    return false;
  }

  // Check if we're at capacity and would exceed max size
  if (arr->size >= MAX_ARRAY_SIZE) {
    set_error_with_warning(err, ERR_ARRAY_FULL,
                           "ERROR: Array has reached maximum size limit\n");
    return false;
  }

  // Resize if needed
  if (arr->size >= arr->capacity) {
    size_t new_capacity = arr->capacity * GROWTH_FACTOR;
    if (!resize_array(arr, new_capacity, err)) {
      return false;
    }
  }

  // Shift elements to the right
  for (size_t i = arr->size; i > index; i--) {
    arr->strings[i] = arr->strings[i - 1];
  }

  // Allocate and insert new string
  arr->strings[index] = malloc((str_len + 1) * sizeof(char));
  if (!arr->strings[index]) {
    set_error_with_warning(
        err, ERR_OUT_OF_MEMORY,
        "ERROR: Failed to allocate memory for inserted string\n");
    return false;
  }

  // Use safe string copy
  int result = snprintf(arr->strings[index], str_len + 1, "%s", str);
  if (result < 0 || (size_t)result >= str_len + 1) {
    free(arr->strings[index]);
    set_error_with_warning(err, ERR_GENERIC,
                           "ERROR: Failed to copy inserted string safely\n");
    return false;
  }

  arr->size++;
  return true;
}

bool remove_string(DynamicStringArray *arr, size_t index, standard_error *err) {
  if (!arr) {
    set_error_with_warning(err, ERR_NULL_POINTER,
                           "ERROR: NULL array pointer in remove_string\n");
    return false;
  }

  if (arr->size == 0) {
    set_error_with_warning(err, ERR_INVALID_STATE,
                           "ERROR: Cannot remove from empty array\n");
    return false;
  }

  if (index >= arr->size) {
    set_error_with_warning(err, ERR_INDEX_OUT_OF_BOUNDS,
                           "ERROR: Index out of bounds in remove_string\n");
    return false;
  }

  // Free the string at index
  free(arr->strings[index]);

  // Shift elements to the left
  for (size_t i = index; i < arr->size - 1; i++) {
    arr->strings[i] = arr->strings[i + 1];
  }

  arr->size--;
  return true;
}

bool set_string(DynamicStringArray *arr, size_t index, const char *str,
                standard_error *err) {
  if (!arr) {
    set_error_with_warning(err, ERR_NULL_POINTER,
                           "ERROR: NULL array pointer in set_string\n");
    return false;
  }

  if (!str) {
    set_error_with_warning(err, ERR_INVALID_ARGS,
                           "ERROR: NULL string pointer in set_string\n");
    return false;
  }

  if (index >= arr->size) {
    set_error_with_warning(err, ERR_INDEX_OUT_OF_BOUNDS,
                           "ERROR: Index out of bounds in set_string\n");
    return false;
  }

  // Check string length
  size_t str_len = strlen(str);
  if (str_len >= MAX_STRING_LENGTH) {
    set_error_with_warning(err, ERR_STRING_TOO_LONG,
                           "ERROR: String exceeds maximum length limit\n");
    return false;
  }

  // Free old string
  free(arr->strings[index]);

  // Allocate and copy new string
  arr->strings[index] = malloc((str_len + 1) * sizeof(char));
  if (!arr->strings[index]) {
    set_error_with_warning(
        err, ERR_OUT_OF_MEMORY,
        "ERROR: Failed to allocate memory for replacement string\n");
    return false;
  }

  // Use safe string copy
  int result = snprintf(arr->strings[index], str_len + 1, "%s", str);
  if (result < 0 || (size_t)result >= str_len + 1) {
    free(arr->strings[index]);
    arr->strings[index] = NULL;
    set_error_with_warning(err, ERR_GENERIC,
                           "ERROR: Failed to copy replacement string safely\n");
    return false;
  }

  return true;
}

char *get_string(DynamicStringArray *arr, size_t index, standard_error *err) {
  if (!arr) {
    set_error_with_warning(err, ERR_NULL_POINTER,
                           "ERROR: NULL array pointer in get_string\n");
    return NULL;
  }

  if (index >= arr->size) {
    set_error_with_warning(err, ERR_INDEX_OUT_OF_BOUNDS,
                           "ERROR: Index out of bounds in get_string\n");
    return NULL;
  }

  return arr->strings[index];
}

char *get_string_copy(DynamicStringArray *arr, size_t index,
                      standard_error *err) {
  if (!arr) {
    set_error_with_warning(err, ERR_NULL_POINTER,
                           "ERROR: NULL array pointer in get_string_copy\n");
    return NULL;
  }

  if (index >= arr->size) {
    set_error_with_warning(err, ERR_INDEX_OUT_OF_BOUNDS,
                           "ERROR: Index out of bounds in get_string_copy\n");
    return NULL;
  }

  size_t str_len = strlen(arr->strings[index]);
  char *copy = malloc((str_len + 1) * sizeof(char));
  if (!copy) {
    set_error_with_warning(
        err, ERR_OUT_OF_MEMORY,
        "ERROR: Failed to allocate memory for string copy\n");
    return NULL;
  }

  // Use safe string copy
  int result = snprintf(copy, str_len + 1, "%s", arr->strings[index]);
  if (result < 0 || (size_t)result >= str_len + 1) {
    free(copy);
    set_error_with_warning(err, ERR_GENERIC,
                           "ERROR: Failed to copy string safely\n");
    return NULL;
  }

  return copy;
}

size_t array_size(DynamicStringArray *arr) { return arr ? arr->size : 0; }

size_t array_capacity(DynamicStringArray *arr) {
  return arr ? arr->capacity : 0;
}

bool is_empty(DynamicStringArray *arr) { return arr ? (arr->size == 0) : true; }

void clear_array(DynamicStringArray *arr) {
  if (!arr)
    return;

  for (size_t i = 0; i < arr->size; i++) {
    free(arr->strings[i]);
  }

  arr->size = 0;
}

void print_array(DynamicStringArray *arr) {
  if (!arr) {
    printf("Array is NULL\n");
    return;
  }

  printf("Array: size=%zu, capacity=%zu\n", arr->size, arr->capacity);
  for (size_t i = 0; i < arr->size; i++) {
    printf("  [%zu]: \"%s\"\n", i, arr->strings[i]);
  }
  printf("\n");
}
