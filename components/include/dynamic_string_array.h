#ifndef DYNAMIC_STRING_ARRAY
#define DYNAMIC_STRING_ARRAY
#include "../typedef/standard_error.h"

#include <stdbool.h>
#include <stddef.h>

#define INITIAL_CAPACITY 20
#define GROWTH_FACTOR 2
#define MAX_ARRAY_SIZE 100000
#define MAX_STRING_LENGTH 4096
typedef struct {
  char **strings;
  size_t size;
  size_t capacity;
} DynamicStringArray;

// Core functions
DynamicStringArray *create_array(standard_error *err);
void destroy_array(DynamicStringArray *arr);

// Array operations
bool add_string(DynamicStringArray *arr, const char *str, standard_error *err);
bool insert_string(DynamicStringArray *arr, size_t index, const char *str,
                   standard_error *err);
bool remove_string(DynamicStringArray *arr, size_t index, standard_error *err);
bool set_string(DynamicStringArray *arr, size_t index, const char *str,
                standard_error *err);

// Access functions
char *get_string(DynamicStringArray *arr, size_t index, standard_error *err);
char *get_string_copy(DynamicStringArray *arr, size_t index,
                      standard_error *err);

// Utility functions
size_t array_size(DynamicStringArray *arr);
size_t array_capacity(DynamicStringArray *arr);
bool is_empty(DynamicStringArray *arr);
void clear_array(DynamicStringArray *arr);
void print_array(DynamicStringArray *arr);

#endif
