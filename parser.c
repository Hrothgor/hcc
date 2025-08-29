
#include "parser.h"
#include "error.h"

#define consume_token_till(p, kind) consume_token_till(p->tokens, &p->cursor, kind)
#define consume_token(p, count) consume_token(p->tokens, &p->cursor, count)

Parser parser_new(TokenVector tokens)
{
    Parser p = {
        .tokens = tokens,
        .cursor = 0,
    };
    return p;
}

bool iskeyword_statement(Token t)
{
    if (t.kind != TOKEN_KEYWORD) return false;

    const char *keywords_statement[] = {
        "do", "else", "for", "goto", "if",
        "return", "switch", "while",
    };

    for (size_t i = 0; i < ARRAY_SIZE(keywords_statement); i++) {
        if (t.text_len != strlen(keywords_statement[i])) continue;
        if (strncmp(t.text, keywords_statement[i], t.text_len) == 0) {
            return true;
        }
    }

    return false;
}

bool iskeyword_type(Token t)
{
    if (t.kind != TOKEN_KEYWORD) return false;

    const char *keywords_type[] = {
        "char", "const", "double", "float", "int",
        "long", "short", "signed", "unsigned", "void",
    };
    for (size_t i = 0; i < ARRAY_SIZE(keywords_type); i++) {
        if (t.text_len != strlen(keywords_type[i])) continue;
        if (strncmp(t.text, keywords_type[i], t.text_len) == 0) {
            return true;
        }
    }

    return false;
}

Errno parse_expresion(Parser *p, Token keyword)
{
    Errno result = 0;
    UNUSED(p);
    UNUSED(keyword);
    return result;
}

// TODO: Handle multi type variable (eg. long long int)
Errno parse_declaration(Parser *p, Token keyword)
{
    Errno result = 0;
    UNUSED(p);
    UNUSED(keyword);

    TokenVector ts = consume_token(p, 2);
    if (vec_size(&ts) < 2) return 0;
    Token identifier = vec_at(&ts, 0);
    Token operator = vec_at(&ts, 1);
    if (identifier.text_len == 0 || operator.text_len == 0) return 0;

    if (operator.kind == TOKEN_OPEN_PAREN) {
        // funciton declaration or definition
        printf("funciton declaration or definition\n");
        // TokenVector ts = consume_token_till(p, TOKEN_SEMICOLON);
        // TokenVector ts = consume_token_till(p, TOKEN_OPENCURLY);
    } else if (operator.kind == TOKEN_SEMICOLON) {
        // variable declaration without init
        printf("variable declaration without init\n");
    } else if (operator.kind == TOKEN_EQUAL) {
        // variable declaration with init
        printf("variable declaration with init\n");
        TokenVector ts = consume_token_till(p, TOKEN_SEMICOLON);
        UNUSED(ts);
    } else {
        return 0;
    }

    return result;
}

Errno parse_statement(Parser *p, Token keyword)
{
    Errno result = 0;
    UNUSED(p);
    UNUSED(keyword);
    return result;
}

Errno parse_keyword(Parser *p, Token keyword)
{
    Errno result = 0;

    if (iskeyword_type(keyword)) {
        result = parse_declaration(p, keyword);
    } else if (iskeyword_statement(keyword)) {
        result = parse_statement(p, keyword);
    }

    return result;
}

Errno parser_parse(Parser *p)
{
    Errno result = ERR_NONE;

    TokenVector ts = {0};
    while ((ts = consume_token(p, 1)).count > 0) {
        Token t = vec_at(&ts, 0);
        p->error_token = t;

        switch (t.kind) {
            case TOKEN_END:
            case TOKEN_COMMENT:
            case TOKEN_PREPROC:
            case TOKEN_SEMICOLON: continue;

            case TOKEN_INVALID: return_defer(ERR_INVALID_TOKEN);
            case TOKEN_SYMBOL: continue; // TODO
            case TOKEN_KEYWORD: {
                Errno err = parse_keyword(p, t);
                if (err) return_defer(err);
                break;
            }
            default: break; //return_defer(ERR_EXPECTED_IDENTIFIER);
        }
    }
    p->error_token = (Token){0};

defer:
    return result;
}