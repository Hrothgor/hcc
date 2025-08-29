
#include "error.h"

void print_error(ErrorCode code, Token error_token)
{
    char msg[256] = {0};

    switch (code)
    {
    case ERR_INVALID_TOKEN:
        sprintf(msg, "Invalid token");
        break;
    case ERR_EXPECTED_IDENTIFIER:
        if (error_token.kind == TOKEN_STRING)
            sprintf(msg, "expected identifier before string constant");
        else if (error_token.kind == TOKEN_NUMBER)
            sprintf(msg, "expected identifier before numeric constant");
        else
            sprintf(msg, "expected identifier before '%.*s'", (int)error_token.text_len, error_token.text);
        break;
    case ERR_INVALID_INCLUDE:
        sprintf(msg, "#include expects \"FILENAME\" or <FILENAME>");
        break;
    case ERR_INVALID_PREPROC:
        sprintf(msg, "Invalid preprocessing directive '%.*s'", (int)error_token.text_len, error_token.text);
        break;
    case ERR_MISSING_DEFINE:
        sprintf(msg, "No macro name given in #define directive");
        break;
    default:
        sprintf(msg, "Unknown error");
        break;
    }

    fprintf(stderr, "%lld:%lld: error: %s\n", error_token.line, error_token.column, msg);
}
