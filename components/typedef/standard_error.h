#ifndef STANDARD_ERROR_H
#define STANDARD_ERROR_H


#define ERR_NONE 0
#define ERR_MKDIR_FAILED 1
#define ERR_WRITE_FILE_FAILED 2
#define ERR_MESSAGE_SIZE 256

typedef struct{
    int code;
    char message[ERR_MESSAGE_SIZE];
} standard_error;

#endif
