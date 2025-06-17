// gitignore_loader.h
#ifndef GITIGNORE_LOADER_H
#define GITIGNORE_LOADER_H

#include <ctype.h>   // For isspace
#include <fnmatch.h> // For fnmatch for pattern matching
#include <stdio.h>   // For FILE, fopen, fclose, fgets, snprintf
#include <stdlib.h>  // For malloc, free
#include <string.h>  // For strlen, strncpy

// Define PATH_MAX if it's not already defined by the system headers.
// This is a common practice to ensure portability.
#ifndef PATH_MAX
#define PATH_MAX 4096
#endif

// Include uthash.h for the hash table functionality.
// Make sure this header file is available in your project.
// You might need to download it from https://troydhanson.github.io/uthash/
#include "uthash.h"

/**
 * @brief Structure to represent an entry in the .gitignore pattern list.
 * It stores the pattern string and a hash handle for UT_hash.
 */
typedef struct ignore_entry {
  char pattern[PATH_MAX]; ///< The .gitignore pattern string
  UT_hash_handle hh;      ///< UT_hash handle for linking entries
} ignore_entry;

// Declare the global hash table for ignore patterns as extern.
// This means the actual definition will be in a .c file.
extern ignore_entry *ignore_patterns;

/**
 * @brief Trims leading and trailing whitespace from a string.
 * @param str The string to trim.
 * @return A pointer to the trimmed string (modifies the input string in place).
 */
char *trim(char *str);

/**
 * @brief Loads .gitignore patterns from a .gitignore file in the specified
 * absolute path. Patterns are added to the global ignore_patterns hash table.
 * @param abs The absolute path to the directory containing the .gitignore file.
 */
void load_gitignore(const char *abs);

/**
 * @brief Checks if a given relative path is ignored based on the loaded
 * patterns.
 * @param rel_path The relative path to check.
 * @return 1 if the path is ignored, 0 otherwise.
 */
int is_ignored(const char *rel_path);

/**
 * @brief Frees all memory allocated for the .gitignore patterns hash table.
 * Should be called when the patterns are no longer needed to prevent memory
 * leaks.
 */
void free_gitignore_patterns();

#endif // GITIGNORE_LOADER_H
