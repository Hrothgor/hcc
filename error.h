#ifndef ERROR_H_
#define ERROR_H_

#include "hcc.h"
#include "lexer.h"

typedef enum {
    ERR_NONE = 0,
    ERR_INVALID_TOKEN,
    ERR_EXPECTED_IDENTIFIER,
    ERR_INVALID_INCLUDE,
    ERR_INVALID_PREPROC,
    ERR_MISSING_DEFINE,
} ErrorCode;

void print_error(ErrorCode code, Token error_token);

#endif // ERROR_H_