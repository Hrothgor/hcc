
#include "preprocessor.h"
#include "error.h"

#define consume_token_till(pp, kind) consume_token_till(pp->tokens, &pp->cursor, kind)
#define consume_token(pp, count) consume_token(pp->tokens, &pp->cursor, count)

void print_defines(Preprocessor *pp)
{
    for (size_t i = 0; i < vec_size(&pp->defines); i++) {
        DefineDirective dd = vec_at(&pp->defines, i);
        printf("Define: %.*s =", (int)dd.name.text_len, dd.name.text);
        for (size_t j = 0; j < vec_size(&dd.value); j++) {
            Token v = vec_at(&dd.value, j);
            printf(" %.*s", (int)v.text_len, v.text);
        }
        printf("\n");
    }
}

Preprocessor preprocessor_new(TokenVector tokens)
{
    Preprocessor pp = {
        .tokens = tokens,
        .cursor = 0,
    };
    return pp;
}

ErrorCode process_include(Preprocessor *pp)
{
    // TODO: Check for cyclic include and prevent infinite recursion
    ErrorCode result = ERR_NONE;
    char *content = NULL;

    TokenVector ts = consume_token(pp, 1);
    if (vec_size(&ts) == 0) return_defer(ERR_INVALID_INCLUDE);
    Token file_token = vec_at(&ts, 0);
    if (file_token.text_len == 0) return_defer(ERR_INVALID_INCLUDE);

    if (file_token.text[0] == '<' && file_token.text[file_token.text_len - 1] == '>') {
        // TODO: Handle angle-bracketed includes
        return ERR_NONE;
    } else if (file_token.text[0] == '\"' && file_token.text[file_token.text_len - 1] == '\"') {
        file_token.text++;
        file_token.text_len -= 2;
    } else {
        return_defer(ERR_INVALID_INCLUDE);
    }

    if (read_sfile(file_token.text, file_token.text_len, &content)) return_defer(ERR_INVALID_INCLUDE);

    Lexer l = lexer_new(content, strlen(content));
    TokenVector newtokens = {0};
    Token t;
    while ((t = lexer_next(&l)).kind != TOKEN_END) {
        vec_push_back(&newtokens, t);
    }
    
    vec_insert_many(&pp->tokens, pp->cursor, newtokens.items, newtokens.count);

    vec_free(&newtokens);

defer:
    garbage_collect(content);
    return result;
}

ErrorCode process_define(Preprocessor *pp)
{
    ErrorCode result = ERR_NONE;
    
    TokenVector ts = consume_token(pp, 1);
    if (vec_size(&ts) == 0) return_defer(ERR_MISSING_DEFINE);
    Token define_token = vec_at(&ts, 0);

    Lexer l = lexer_new(define_token.text, define_token.text_len);
    TokenVector tokens = {0};
    Token t;
    while ((t = lexer_next(&l)).kind != TOKEN_END) {
        vec_push_back(&tokens, t);
    }

    if (vec_size(&tokens) == 0) return_defer(ERR_MISSING_DEFINE);

    DefineDirective dd = {0};
    dd.name = vec_at(&tokens, 0);
    if (vec_size(&tokens) > 1) {
        TokenVector values = {0};
        vec_append_many(&values, tokens.items + 1, tokens.count - 1);
        dd.value = values;
    }
    vec_push_back(&pp->defines, dd);

defer:
    vec_free(&tokens);
    return result;
}

void try_replace_define(Preprocessor *pp, Token symbol, size_t symbol_index)
{
    if (symbol.text_len == 0) return;

    for (size_t i = 0; i < pp->defines.count; i++) {
        DefineDirective dd = vec_at(&pp->defines, i);
        if (compare_token(dd.name, symbol)) {
            vec_erase(&pp->tokens, symbol_index);
            vec_insert_many(&pp->tokens, symbol_index, dd.value.items, dd.value.count);
            pp->cursor = symbol_index;
        }
    }
}

ErrorCode process_directive(Preprocessor *pp, Token directive)
{
    if (sstrcmp(directive.text, "#include", directive.text_len) == 0) {
        return process_include(pp);
    }
    if (sstrcmp(directive.text, "#define", directive.text_len) == 0) {
        return process_define(pp);
    }

    return ERR_NONE;
    // return ERR_INVALID_PREPROC;
}

ErrorCode preprocessor_process(Preprocessor *pp)
{
    ErrorCode result = ERR_NONE;

    TokenVector ts = {0};
    while ((ts = consume_token(pp, 1)).count > 0) {
        Token t = vec_at(&ts, 0);
        pp->error_token = t;

        if (t.kind == TOKEN_PREPROC) {
            ErrorCode err = process_directive(pp, t);
            if (err) return_defer(err);
        } else if (t.kind == TOKEN_SYMBOL) {
            try_replace_define(pp, t, pp->cursor - 1);
        }
 
    }
    pp->error_token = (Token){0};

    print_defines(pp);

defer:
    for (size_t i = 0; i < vec_size(&pp->defines); i++) {
        vec_free(&(vec_at(&pp->defines, i)).value);
    }
    vec_free(&pp->defines);
    return result;
}