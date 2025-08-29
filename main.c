#include "hcc.h"
#include "lexer.h"
#include "parser.h"
#include "preprocessor.h"
#include "error.h"

int main(int argc, char **argv)
{
    char *program = shift(argv, argc);
    char *file_path = shift(argv, argc);
    if (!file_path) {
        printf("USAGE: %s [FILE]\n", program);
        printf("\tFILE: file path to the code to compile\n");
        return 1;
    }

    char *content = NULL;
    Errno err = read_file(file_path, &content);
    if (err) {
        fprintf(stderr, "ERROR: failed to load `%s` file: %s\n", file_path, strerror(errno));
        return 1;
    }

    ErrorCode result = ERR_NONE;

    Lexer l = lexer_new(content, strlen(content));
    TokenVector tokens = {0};
    Token t;
    while ((t = lexer_next(&l)).kind != TOKEN_END) {
        vec_push_back(&tokens, t);
    }
    vec_push_back(&tokens, t);

    Preprocessor pp = preprocessor_new(tokens);
    ErrorCode cerr = preprocessor_process(&pp);

    if (cerr) { print_error(cerr, pp.error_token); return_defer(result); }


    // Parser p = parser_new(pp.tokens);
    // cerr = parser_parse(&p);

    // if (cerr) { print_error(cerr, p.error_token); return_defer(result); }

defer:
    garbage_collect(content);
    vec_free(&tokens);

    garbage_release();
    return result;
}