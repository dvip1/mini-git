#ifndef ERROR_HANDLING_H
#define ERROR_HANDLING_H
#include "../typedef/standard_error.h"

void set_error(standard_error *err, int code, const char *fmt, ...);
void clear_error(standard_error *err);
void set_error_with_warning(standard_error *err, int code, const char *fmt,
                            ...);
void set_error_and_exit(standard_error *err, int code, const char *fmt, ...);

#endif
