#ifndef PARSER_H_
#define PARSER_H_

#include "hcc.h"
#include "lexer.h"

typedef struct {
    TokenVector tokens;
    size_t cursor;

    // Error handling
    Token error_token;
} Parser;

Parser parser_new(TokenVector tokens);
Errno parser_parse(Parser *parser);

#endif // PARSER_H_