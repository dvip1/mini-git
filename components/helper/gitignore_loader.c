#include "../include/gitignore_loader.h"
#include "../include/limits.h"
#include <ctype.h>   // isspace
#include <fnmatch.h> // fnmatch for pattern matching
#include <stdio.h>
#include <string.h>
#include <strings.h>
#define IGNORE_PATH_LIMIT PATH_MAX

ignore_entry *ignore_patterns = NULL;

char *trim(char *str) {

  char *end;

  while (isspace((unsigned char)*str))
    str++;

  if (*str == 0)
    return str;

  end = str + strlen(str) - 1;

  while (end > str && isspace((unsigned char)*end))
    end--;

  *(end + 1) = 0;

  return str;
}

char *normalize_path(const char *path) {
  static char normalized[PATH_MAX];
  char *result = normalized;
  const char *src = path;

  // Skip leading "./"
  if (strncmp(src, "./", 2) == 0) {
    src += 2;
  }

  // Copy and normalize
  while (*src && (result - normalized) < PATH_MAX - 1) {
    if (*src == '/' && *(src + 1) == '/') {
      // Skip double slashes
      src++;
      continue;
    }
    *result++ = *src++;
  }
  *result = '\0';

  // Remove trailing slash unless it's just "/"
  int len = strlen(normalized);
  if (len > 1 && normalized[len - 1] == '/') {
    normalized[len - 1] = '\0';
  }

  return normalized;
}

// Helper function to unescape gitignore patterns
char *unescape_pattern(const char *pattern) {
  static char unescaped[PATH_MAX];
  const char *src = pattern;
  char *dst = unescaped;

  while (*src && (dst - unescaped) < PATH_MAX - 1) {
    if (*src == '\\' && *(src + 1)) {
      // Handle escaped characters
      src++;           // Skip the backslash
      *dst++ = *src++; // Copy the escaped character
    } else {
      *dst++ = *src++;
    }
  }
  *dst = '\0';

  return unescaped;
}

// Comprehensive recursive pattern matching for **
int match_recursive_pattern(const char *pattern, const char *path) {
  char pattern_copy[PATH_MAX];
  char path_copy[PATH_MAX];
  snprintf(pattern_copy, sizeof(pattern_copy), "%s", pattern);
  snprintf(path_copy, sizeof(path_copy), "%s", path);

  // Case 1: **/something - matches something at any depth
  if (strncmp(pattern_copy, "**/", 3) == 0) {
    char *after_stars = pattern_copy + 3;

    // Try matching the remainder against the full path
    if (fnmatch(after_stars, path_copy, 0) == 0) {
      return 1;
    }

    // Try matching against each path component
    char *path_part = path_copy;
    while ((path_part = strchr(path_part, '/')) != NULL) {
      path_part++; // Skip the '/'
      if (fnmatch(after_stars, path_part, 0) == 0) {
        return 1;
      }
    }

    // Try matching just the basename
    char *basename = strrchr(path_copy, '/');
    if (basename) {
      basename++; // Skip the '/'
      if (fnmatch(after_stars, basename, 0) == 0) {
        return 1;
      }
    } else {
      // No slash in path, try matching whole path
      if (fnmatch(after_stars, path_copy, 0) == 0) {
        return 1;
      }
    }
    return 0;
  }

  // Case 2: something/** - matches something and everything under it
  int pattern_len = strlen(pattern_copy);
  if (pattern_len >= 3 && strcmp(pattern_copy + pattern_len - 3, "/**") == 0) {
    // Remove the /** suffix
    pattern_copy[pattern_len - 3] = '\0';

    // Check if path starts with the prefix
    int prefix_len = strlen(pattern_copy);
    if (strncmp(path_copy, pattern_copy, prefix_len) == 0) {
      // Path must either be exact match or have / after prefix
      if (path_copy[prefix_len] == '\0' || path_copy[prefix_len] == '/') {
        return 1;
      }
    }
    return 0;
  }

  // Case 3: prefix/**/suffix - complex recursive matching
  char *star_pos = strstr(pattern_copy, "/**/");
  if (star_pos != NULL) {
    *star_pos = '\0'; // Split pattern at /**
    char *prefix = pattern_copy;
    char *suffix = star_pos + 4; // Skip /**/

    int prefix_len = strlen(prefix);

    // Path must start with prefix
    if (strncmp(path_copy, prefix, prefix_len) != 0) {
      return 0;
    }

    // If no suffix, just check prefix match
    if (strlen(suffix) == 0) {
      return (path_copy[prefix_len] == '\0' || path_copy[prefix_len] == '/');
    }

    // Try matching suffix at various positions in the remaining path
    char *remaining = path_copy + prefix_len;
    if (*remaining == '/') {
      remaining++; // Skip the leading slash
    }

    // Try matching suffix against remaining path and all sub-paths
    char *search_pos = remaining;
    do {
      if (fnmatch(suffix, search_pos, 0) == 0) {
        return 1;
      }

      // Find next path component
      search_pos = strchr(search_pos, '/');
      if (search_pos) {
        search_pos++; // Skip the '/'
      }
    } while (search_pos != NULL);

    return 0;
  }

  // Case 4: ** in middle of filename (rare but possible)
  if (strstr(pattern_copy, "**") != NULL) {
    // Replace ** with * and try normal matching as fallback
    char simple_pattern[PATH_MAX];
    snprintf(simple_pattern, sizeof(simple_pattern), "%s", pattern_copy);

    // Replace all ** with *
    char *double_star;
    while ((double_star = strstr(simple_pattern, "**")) != NULL) {
      memmove(double_star + 1, double_star + 2, strlen(double_star + 2) + 1);
    }

    return fnmatch(simple_pattern, path_copy, 0) == 0;
  }

  return 0;
}

// Helper to check if path is a directory
int is_directory_path(const char *path) {
  int len = strlen(path);
  return (len > 0 && path[len - 1] == '/');
}

int pattern_matches(const char *pattern, const char *path, int is_dir) {
  char *unescaped = unescape_pattern(pattern);
  char test_pattern[PATH_MAX];
  char test_path[PATH_MAX];

  snprintf(test_pattern, sizeof(test_pattern), "%s", unescaped);
  snprintf(test_path, sizeof(test_path), "%s", path);

  // Handle directory-only patterns (ending with /)
  int pattern_is_dir_only = (strlen(test_pattern) > 0 &&
                             test_pattern[strlen(test_pattern) - 1] == '/');

  if (pattern_is_dir_only) {
    // Remove trailing slash from pattern
    test_pattern[strlen(test_pattern) - 1] = '\0';

    // Only match directories
    if (!is_dir) {
      return 0;
    }
  }

  // Handle leading slash (absolute patterns)
  if (test_pattern[0] == '/') {
    // Remove leading slash and only match from root
    memmove(test_pattern, test_pattern + 1, strlen(test_pattern));

    // For absolute patterns, don't allow subdirectory matches
    return fnmatch(test_pattern, test_path, 0) == 0;
  }

  // Handle recursive patterns (**)
  if (strstr(test_pattern, "**") != NULL) {
    return match_recursive_pattern(test_pattern, test_path);
  }

  // Regular pattern matching
  if (fnmatch(test_pattern, test_path, 0) == 0) {
    return 1;
  }

  // Try matching basename only
  char *basename = strrchr(test_path, '/');
  if (basename) {
    basename++; // Skip the '/'
    if (fnmatch(test_pattern, basename, 0) == 0) {
      return 1;
    }
  }

  // Try adding directory slash
  if (is_dir) {
    char dir_path[PATH_MAX];
    snprintf(dir_path, sizeof(dir_path), "%s/", test_path);
    if (fnmatch(test_pattern, dir_path, 0) == 0) {
      return 1;
    }
  }

  return 0;
}

void load_gitignore(const char *abs) {
  char gitignore_path[PATH_MAX];
  snprintf(gitignore_path, sizeof(gitignore_path), "%s/.gitignore", abs);

  FILE *fp = fopen(gitignore_path, "r");
  if (!fp)
    return;

  char line[PATH_MAX];
  while (fgets(line, sizeof(line), fp)) {
    char *trimmed = trim(line);

    // Skip empty lines and comments (but not escaped comments)
    if (*trimmed == '\0' ||
        (*trimmed == '#' && (trimmed == line || *(trimmed - 1) != '\\'))) {
      continue;
    }

    // Skip if pattern is too long
    if (strlen(trimmed) >= PATH_MAX) {
      continue;
    }

    ignore_entry *entry = malloc(sizeof(ignore_entry));
    if (!entry) {
      continue;
    }

    strncpy(entry->pattern, trimmed, PATH_MAX - 1);
    entry->pattern[PATH_MAX - 1] = '\0';

    HASH_ADD_STR(ignore_patterns, pattern, entry);
  }
  fclose(fp);
  ignore_entry *git_entry = malloc(sizeof(ignore_entry));
  if (git_entry) {
    snprintf(git_entry->pattern, sizeof(git_entry->pattern), ".git");
    HASH_ADD_STR(ignore_patterns, pattern, git_entry);
  }
}

int is_ignored(const char *rel_path) {
  // Always ignore .mini-git directory
  if (strstr(rel_path, ".mini-git") != NULL) {
    return 1;
  }

  // Normalize the path
  char *clean_path = normalize_path(rel_path);

  // Determine if this is a directory path
  int is_dir = is_directory_path(rel_path) || is_directory_path(clean_path);

  // Track matches and negations
  int matched = 0;
  int negated = 0;

  ignore_entry *entry, *tmp;
  HASH_ITER(hh, ignore_patterns, entry, tmp) {
    char *pattern = entry->pattern;

    // Handle negation patterns (starting with !)
    if (pattern[0] == '!') {
      // This is a negation pattern
      if (pattern_matches(pattern + 1, clean_path, is_dir)) {
        negated = 1;
      }
    } else {
      // Regular pattern
      if (pattern_matches(pattern, clean_path, is_dir)) {
        matched = 1;
      }
    }
  }

  // Return result: ignored if matched and not negated
  return matched && !negated;
}

void free_gitignore_patterns() {

  ignore_entry *entry, *tmp;

  HASH_ITER(hh, ignore_patterns, entry, tmp) {

    HASH_DEL(ignore_patterns, entry);

    free(entry);
  }
}
