#ifndef STANDARD_ERROR_H
#define STANDARD_ERROR_H
#include <stdio.h>

#define ERR_NONE 0
#define ERR_UNKNOWN -1
#define ERR_INVALID_ARGS -2
#define ERR_OUT_OF_MEMORY -3
#define ERR_BUFFER_TOO_SMALL -4
#define ERR_GENERIC -5
#define ERR_NULL_POINTER -6
#define ERR_INDEX_OUT_OF_BOUNDS -7
#define ERR_INVALID_STATE -8
#define ERR_TIMEOUT -9
#define ERR_PERMISSION_DENIED -10
#define ERR_NOT_IMPLEMENTED -11
#define ERR_OPERATION_CANCELLED -12

// Legacy positive error codes (consider migrating to negative)
#define ERR_CREATE_FILE 3
#define ERR_COMMAND 4
#define ERR_MESSAGE_SIZE 256

// File system errors (-100 to -199)
#define ERR_FILE_NOT_FOUND -100
#define ERR_ACCESS_DENIED -101
#define ERR_FILE_EXISTS -102
#define ERR_MKDIR_FAILED -103
#define ERR_WRITE_FAILED -104
#define ERR_READ_FAILED -105
#define ERR_IS_A_DIRECTORY -106
#define ERR_NOT_A_DIRECTORY -107
#define ERR_DISK_FULL -108
#define ERR_FILE_LOCKED -109
#define ERR_PATH_TOO_LONG -110
#define ERR_INVALID_PATH -111

// Network errors (-200 to -299)
#define ERR_HOST_NOT_FOUND -200
#define ERR_CONNECTION_REFUSED -201
#define ERR_CONNECTION_TIMED_OUT -202
#define ERR_NETWORK_UNREACHABLE -203
#define ERR_SSL_HANDSHAKE_FAILED -204
#define ERR_HTTP_ERROR -205
#define ERR_AUTHENTICATION_FAILED -206
#define ERR_PROTOCOL_ERROR -207
#define ERR_CERTIFICATE_ERROR -208
#define ERR_PROXY_ERROR -209

// Repository errors (-300 to -399)
#define ERR_REPOSITORY_NOT_FOUND -300
#define ERR_INVALID_REPO_URL -301
#define ERR_REMOTE_HEAD_NOT_FOUND -302
#define ERR_NOT_A_GIT_REPOSITORY -303
#define ERR_CLONE_FAILED -304
#define ERR_INVALID_RESPONSE -305
#define ERR_BRANCH_NOT_FOUND -306
#define ERR_MERGE_CONFLICT -307
#define ERR_UNCOMMITTED_CHANGES -308

// CUSTOM errors (-400 to -499)

// Database errors (-500 to -599)
#define ERR_DB_CONNECTION_FAILED -500
#define ERR_DB_QUERY_FAILED -501
#define ERR_DB_TABLE_NOT_FOUND -502
#define ERR_DB_CONSTRAINT_VIOLATION -503
#define ERR_DB_TRANSACTION_FAILED -504

// Parser/Format errors (-600 to -699)
#define ERR_PARSE_ERROR -600
#define ERR_INVALID_FORMAT -601
#define ERR_MALFORMED_DATA -602
#define ERR_ENCODING_ERROR -603
#define ERR_COMPRESSION_ERROR -604

// Threading/Concurrency errors (-700 to -799)
#define ERR_THREAD_CREATE_FAILED -700
#define ERR_MUTEX_LOCK_FAILED -701
#define ERR_DEADLOCK_DETECTED -702
#define ERR_RESOURCE_BUSY -703

// Validation errors (-800 to -899)
#define ERR_VALIDATION_FAILED -800
#define ERR_CHECKSUM_MISMATCH -801
#define ERR_VERSION_MISMATCH -802
#define ERR_SIGNATURE_INVALID -803
#define ERR_ARRAY_FULL -850
#define ERR_STRING_TOO_LONG -851

typedef struct {
  int code;
  char message[ERR_MESSAGE_SIZE];
} standard_error;

#define SET_ERROR(err, code, fmt, ...)                                         \
  do {                                                                         \
    if ((err)) {                                                               \
      (err)->code = ((code));                                                  \
      snprintf((err)->message, ERR_MESSAGE_SIZE, (fmt), ##__VA_ARGS__);        \
    }                                                                          \
  } while (0)

#define CLEAR_ERROR(err)                                                       \
  do {                                                                         \
    if ((err)) {                                                               \
      (err)->code = ERR_NONE;                                                  \
      (err)->message[0] = '\0';                                                \
    }                                                                          \
  } while (0)

#endif
