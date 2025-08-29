#ifndef LEXER_H_
#define LEXER_H_

#include "hcc.h"

typedef enum {
    TOKEN_END = 0,
    TOKEN_INVALID,

    // Literals tokens
    TOKEN_ARROW, TOKEN_OPEN_PAREN, TOKEN_CLOSE_PAREN, TOKEN_OPEN_CURLY,
    TOKEN_CLOSE_CURLY, TOKEN_SEMICOLON, TOKEN_COMMA, TOKEN_DOT, TOKEN_MINUS,
    TOKEN_PLUS, TOKEN_SLASH, TOKEN_STAR, TOKEN_EQUAL, TOKEN_BANG,
    TOKEN_BANG_EQUAL, TOKEN_EQUAL_EQUAL, TOKEN_GREATER, TOKEN_GREATER_EQUAL,
    TOKEN_LESS, TOKEN_LESS_EQUAL,

    // Complex tokens
    TOKEN_SYMBOL, TOKEN_COMMENT, TOKEN_STRING, TOKEN_NUMBER,

    // Language-specific tokens
    TOKEN_PREPROC, TOKEN_KEYWORD,
} Token_Kind;
const char *token_kind_name(Token_Kind kind);

typedef struct {
    Token_Kind kind;
    const char *text;
    size_t text_len;
    // For error reporting
    size_t line;
    size_t column;
} Token;
vec_define(TokenVector, Token);

TokenVector consume_token_till(TokenVector tokens, size_t *cursor, Token_Kind kind);
TokenVector consume_token(TokenVector tokens, size_t *cursor, size_t count);
bool compare_token(Token t1, Token t2);

typedef enum {
    GENERIC_MODE = 0,
    PREPROC_MODE,
} Lexer_Mode;

typedef struct {
    const char *content;
    size_t content_len;
    size_t cursor;
    Lexer_Mode mode;
    // For error reporting
    size_t line;
    size_t column;
} Lexer;

Lexer lexer_new(const char *content, size_t content_len);
Token lexer_next(Lexer *l);

#endif // LEXER_H_