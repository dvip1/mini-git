#ifndef STANDARD_ERROR_H
#define STANDARD_ERROR_H

// generic
#define ERR_WRITE_FILE_FAILED 2
#define ERR_CREATE_FILE 3
#define ERR_COMMAND 4
#define ERR_MESSAGE_SIZE 256
#define FILE_NOT_FOUND 404

#define ERR_NONE 0
#define ERR_UNKNOWN -1
#define ERR_INVALID_ARGS -2
#define ERR_OUT_OF_MEMORY -3
#define ERR_BUFFER_TOO_SMALL -4
#define ERR_GENERIC -5

// file system error
#define ERR_FILE_NOT_FOUND -100
#define ERR_ACCESS_DENIED -101
#define ERR_FILE_EXISTS -102
#define ERR_MKDIR_FAILED -103
#define ERR_WRITE_FAILED -104
#define ERR_READ_FAILED -105
#define ERR_IS_A_DIRECTORY -106
#define ERR_NOT_A_DIRECTORY -107
#define ERR_DISK_FULL -108
//
// Network related
//
#define ERR_HOST_NOT_FOUND -200
#define ERR_CONNECTION_REFUSED -201
#define ERR_CONNECTION_TIMED_OUT -202
#define ERR_NETWORK_UNREACHABLE -203
#define ERR_SSL_HANDSHAKE_FAILED -204
#define ERR_HTTP_ERROR -205
#define ERR_AUTHENTICATION_FAILED -206

// REpository related
#define ERR_REPOSITORY_NOT_FOUND -300
#define ERR_INVALID_REPO_URL -301
#define ERR_REMOTE_HEAD_NOT_FOUND -302
#define ERR_NOT_A_GIT_REPOSITORY -303
#define ERR_CLONE_FAILED -304
#define ERR_INVALID_RESPONSE -305

typedef struct {
  int code;
  char message[ERR_MESSAGE_SIZE];
} standard_error;

#define SET_ERROR(err, code, fmt, ...)                                         \
  do {                                                                         \
    if ((err)) {                                                               \
      (err)->code = (code);                                                    \
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
