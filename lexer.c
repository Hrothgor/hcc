#include "lexer.h"
#include "hcc.h"

typedef struct {
    Token_Kind kind;
    const char *text;
} Literal_Token;

const Literal_Token literal_tokens[] = {
    { .kind = TOKEN_ARROW,          .text = "->" },
    { .kind = TOKEN_OPEN_PAREN,     .text = "(" },
    { .kind = TOKEN_CLOSE_PAREN,    .text = ")" },
    { .kind = TOKEN_OPEN_CURLY,     .text = "{" },
    { .kind = TOKEN_CLOSE_CURLY,    .text = "}" },
    { .kind = TOKEN_SEMICOLON,      .text = ";" },
    { .kind = TOKEN_COMMA,          .text = "," },
    { .kind = TOKEN_DOT,            .text = "." },
    { .kind = TOKEN_MINUS,          .text = "-" },
    { .kind = TOKEN_PLUS,           .text = "+" },
    { .kind = TOKEN_SLASH,          .text = "/" },
    { .kind = TOKEN_STAR,           .text = "*" },
    { .kind = TOKEN_EQUAL,          .text = "=" },
    { .kind = TOKEN_BANG_EQUAL,     .text = "!=" },
    { .kind = TOKEN_EQUAL_EQUAL,    .text = "==" },
    { .kind = TOKEN_GREATER,        .text = ">" },
    { .kind = TOKEN_GREATER_EQUAL,  .text = ">=" },
    { .kind = TOKEN_LESS,           .text = "<" },
    { .kind = TOKEN_LESS_EQUAL,     .text = "<=" },
};

const char *keywords[] = {
    "auto", "break", "case", "char", "const", "continue", "default", "do",
    "double", "else", "enum", "extern", "float", "for", "goto", "if",
    "int", "long", "register", "return", "short", "signed", "sizeof", "static",
    "struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while",
};

const char *token_kind_name(Token_Kind kind)
{
    switch (kind)
    {
    case TOKEN_END:
        return "end of content";
    case TOKEN_INVALID:
        return "invalid token";

    case TOKEN_ARROW:
        return "arrow";
    case TOKEN_OPEN_PAREN:
        return "open paren";
    case TOKEN_CLOSE_PAREN:
        return "close paren";
    case TOKEN_OPEN_CURLY:
        return "open curly";
    case TOKEN_CLOSE_CURLY:
        return "close curly";
    case TOKEN_SEMICOLON:
        return "semicolon";
    case TOKEN_COMMA:
        return "comma";
    case TOKEN_DOT:
        return "dot";
    case TOKEN_MINUS:
        return "minus";
    case TOKEN_PLUS:
        return "plus";
    case TOKEN_STAR:
        return "star";
    case TOKEN_EQUAL:
        return "equal";
    case TOKEN_BANG:
        return "bang";
    case TOKEN_BANG_EQUAL:
        return "bang equal";
    case TOKEN_EQUAL_EQUAL:
        return "equal equal";
    case TOKEN_GREATER:
        return "greater";
    case TOKEN_GREATER_EQUAL:
        return "greater equal";
    case TOKEN_LESS:
        return "less";
    case TOKEN_LESS_EQUAL:
        return "less equal";

    case TOKEN_SYMBOL:
        return "symbol";
    case TOKEN_COMMENT:
        return "comment";
    case TOKEN_STRING:
        return "string";
    case TOKEN_NUMBER:
        return "number";

    case TOKEN_PREPROC:
        return "preprocessor directive";
    case TOKEN_KEYWORD:
        return "keyword";
    default:
        UNREACHABLE("token_kind_name");
        break;
    }

    return NULL;
}

TokenVector consume_token_till(TokenVector tokens, size_t *cursor, Token_Kind kind)
{
    TokenVector retTokens = {0};
    if (*cursor + 1 > vec_size(&tokens)) return retTokens;

    retTokens.items = tokens.items + *cursor;
    while (*cursor < vec_size(&tokens) && vec_at(&tokens, *cursor).kind != kind) {
        printf("%s: [%.*s]\n", token_kind_name(vec_at(&tokens, *cursor).kind), (int)vec_at(&tokens, *cursor).text_len, vec_at(&tokens, *cursor).text);
        retTokens.count++;
        (*cursor)++;
    }

    return retTokens;
}

TokenVector consume_token(TokenVector tokens, size_t *cursor, size_t count)
{
    TokenVector retTokens = {0};
    if (*cursor + count > vec_size(&tokens)) return retTokens;

    for (size_t i = 0; i < count; i++) {
        Token t = vec_at(&tokens, *cursor + i);
        printf("%s: [%.*s]\n", token_kind_name(t.kind), (int)t.text_len, t.text);
    }

    retTokens.items = tokens.items + *cursor;
    retTokens.count = count;

    *cursor += count;

    return retTokens;
}

bool compare_token(Token t1, Token t2)
{
    if (t1.text_len != t2.text_len) return false;

    for (size_t i = 0; i < t1.text_len; i++) {
        if (t1.text[i] != t2.text[i]) return false;
    }
    return true;
}

Lexer lexer_new(const char *content, size_t content_len)
{
    Lexer l = {
        .content = content,
        .content_len = content_len,
        .cursor = 0,
        .mode = GENERIC_MODE,
        .line = 1,
        .column = 1,
    };
    return l;
}

bool isnumber_start(char c)
{
    return isdigit(c);
}

bool isnumber(char c)
{
    return isdigit(c);
}

bool issymbol_start(char c)
{
    return isalpha(c) || c == '_';
}

bool issymbol(char c)
{
    return isalnum(c) || c == '_';
}

bool iskeyword(const char *str, size_t len)
{
    for (size_t i = 0; i < ARRAY_SIZE(keywords); i++) {
        if (len != strlen(keywords[i])) continue;
        if (strncmp(str, keywords[i], len) == 0) {
            return true;
        }
    }
    return false;
}

bool lexer_start_with(const char *prefix, Lexer *l)
{
    size_t prefix_len = strlen(prefix);

    if (prefix_len == 0) return true;
    if (l->cursor + prefix_len - 1 >= l->content_len) return false;
    
    for (size_t i = 0; i < prefix_len; i++) {
        if (l->content[l->cursor + i] != prefix[i]) return false;
    }
    return true;
}

void lexer_chop_char(Lexer *l, size_t count)
{
    for (size_t i = 0; i < count; ++i) {
        if (l->cursor < l->content_len)
        {
            char c = l->content[l->cursor];
            l->cursor++;
            if (c == '\n') {
                l->line++;
                l->column = 1;
            } else {
                l->column++;
            }
        }
    }
}

void trim_left(Lexer *l)
{
    while (l->cursor < l->content_len && isspace(l->content[l->cursor])) lexer_chop_char(l, 1);
}

void trim_left_until_eol(Lexer *l)
{
    while (l->cursor < l->content_len && (isspace(l->content[l->cursor]) && l->content[l->cursor] != '\n')) lexer_chop_char(l, 1);
}

Token lexer_next(Lexer *l)
{
    if (l->mode == GENERIC_MODE) trim_left(l);
    else if (l->mode == PREPROC_MODE) trim_left_until_eol(l);

    Token token = {
        .text = &l->content[l->cursor],
        .text_len = 0,
        .kind = TOKEN_END,
        .line = l->line,
        .column = l->column,
    };

    if (l->cursor >= l->content_len) return token;

    // Handle preprocessor directives
    if (l->mode == PREPROC_MODE) {
        token.kind = TOKEN_SYMBOL;
        l->mode = GENERIC_MODE;
        // TODO: HANDLE MULTILINE DIRECTIVE
        while (l->cursor < l->content_len && l->content[l->cursor] != '\n')
        {
            lexer_chop_char(l, 1);
        }
        token.text_len = &l->content[l->cursor] - token.text;
        return token;
    }

    // Handle preprocessor symbols
    if (l->content[l->cursor] == '#')
    {
        token.kind = TOKEN_PREPROC;
        l->mode = PREPROC_MODE;
        while (l->cursor < l->content_len && !isspace(l->content[l->cursor]))
        {
            lexer_chop_char(l, 1);
        }
        token.text_len = &l->content[l->cursor] - token.text;
        return token;
    }

    // Handle string
    if (l->content[l->cursor] == '"')
    {
        lexer_chop_char(l, 1);
        token.kind = TOKEN_STRING;
        while (l->cursor < l->content_len && l->content[l->cursor] != '"')
        {
            lexer_chop_char(l, 1);
        }
        lexer_chop_char(l, 1);
        token.text_len = &l->content[l->cursor] - token.text;
        return token;
    }

    // Handle comments
    if (lexer_start_with("//", l))
    {
        token.kind = TOKEN_COMMENT;
        while (l->cursor < l->content_len && l->content[l->cursor] != '\n')
        {
            lexer_chop_char(l, 1);
        }
        token.text_len = &l->content[l->cursor] - token.text;
        return token;
    }

    // Handle symbols
    if (issymbol_start(l->content[l->cursor]))
    {
        token.kind = TOKEN_SYMBOL;
        while (l->cursor < l->content_len && issymbol(l->content[l->cursor]))
        {
            lexer_chop_char(l, 1);
        }
        token.text_len = &l->content[l->cursor] - token.text;
        // Check if the symbol is a keyword
        if (iskeyword(token.text, token.text_len)) {
            token.kind = TOKEN_KEYWORD;
        }
        return token;
    }

    // Handle numbers
    if (isnumber_start(l->content[l->cursor]))
    {
        token.kind = TOKEN_NUMBER;
        while (l->cursor < l->content_len && isnumber(l->content[l->cursor]))
        {
            lexer_chop_char(l, 1);
        }
        token.text_len = &l->content[l->cursor] - token.text;
        return token;
    }

    // Handle literal tokens
    for (size_t i = 0; i < ARRAY_SIZE(literal_tokens); i++) {
        if (lexer_start_with(literal_tokens[i].text, l)) {
            size_t token_len = strlen(literal_tokens[i].text);
            lexer_chop_char(l, token_len);
            token.kind = literal_tokens[i].kind;
            token.text_len = &l->content[l->cursor] - token.text;
            return token;
        }
    }

    // Handle invalid tokens
    token.kind = TOKEN_INVALID;
    while (l->cursor < l->content_len && !isspace(l->content[l->cursor]))
    {
        lexer_chop_char(l, 1);
    }
    token.text_len = &l->content[l->cursor] - token.text;
    return  token;
}