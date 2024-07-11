#include <stdio.h>
#include <stdlib.h>
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
    TokenKind mKind;
    Token *mNext;
    int val;
    char *str;
};

Token *token;

void error(char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

bool consume(char op)
{
    if (token->mKind != TokenKind::TK_RESERVED || token->str[0] != op)
    {
        return false;
    }

    token = token->mNext;
    return true;
}

void expect(char op)
{
    if (token->mKind != TokenKind::TK_RESERVED || token->str[0] != op)
    {
        error("'%c'ではありません。", op);
    }

    token = token->mNext;
}

int expect_number()
{
    if (token->mKind != TokenKind::TK_NUM)
    {
        error("数ではありません。");
    }

    int val = token->val;
    token = token->mNext;
    return val;
}

bool at_end()
{
    return token->mKind == TokenKind::TK_EOF;
}

Token *new_token(TokenKind kind, Token *cur, char *str)
{
    Token *token = reinterpret_cast<Token *>(calloc(1, sizeof(Token)));
    token->mKind = kind;
    token->str = str;
    cur->mNext = token;
    return token;
}

Token *tokenize(char *p)
{
    Token head;
    head.mNext = nullptr;
    Token *cur = &head;

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (*p == '+' || *p == '-')
        {
            cur = new_token(TokenKind::TK_RESERVED, cur, p);
            p++;
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TokenKind::TK_NUM, cur, p);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error("トークナイズ出来ませんでした。");
    }

    new_token(TokenKind::TK_EOF, cur, p);
    return head.mNext;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引き数の個数が正しくありません。");
        return 1;
    }

    token = tokenize(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("    mov rax, %ld\n", expect_number());

    while (!at_end())
    {
        if (consume('+'))
        {
            printf("    add rax, %ld\n", expect_number());
            continue;
        }

        expect('-');
        printf("    sub rax, %ld\n", expect_number());
    }

    printf("    ret\n");
    return 0;
}