#ifndef PREPROCESSOR_H_
#define PREPROCESSOR_H_

#include "hcc.h"
#include "lexer.h"
#include "error.h"

typedef struct {
    Token name;
    TokenVector value;
} DefineDirective;
vec_define(DefineDirectiveVector, DefineDirective);

typedef struct {
    TokenVector tokens;
    size_t cursor;

    DefineDirectiveVector defines;

    // Error handling
    Token error_token;
} Preprocessor;

Preprocessor preprocessor_new(TokenVector tokens);
ErrorCode preprocessor_process(Preprocessor *preprocessor);

#endif // PREPROCESSOR_H_