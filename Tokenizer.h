#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum class TokenKind
{
    TK_RESERVED,
    TK_NUM,
    TK_EOF,
};

class Token
{
public:
    Token(char* user_input) : token(nullptr) , user_input(user_input){}
    void tokenize();

    bool consume(char op);
    void expect(char op);
    int expect_number();
    bool at_end() const;

private:
    TokenKind mKind;
    Token *mNext;
    int val;
    char *str;

    Token *token;
    char *user_input;

    Token *new_token(TokenKind kind, Token *cur, char *str);

    void error(char *fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
        exit(1);
    }

    void error_at(char *location, char *fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);

        int pos = location - user_input;
        fprintf(stderr, "%s\n", user_input);
        fprintf(stderr, "%*s", pos, " ");
        fprintf(stderr, "^ ");
        vfprintf(stderr, fmt, ap);
        fprintf(stderr, "\n");
        exit(1);
    }
};