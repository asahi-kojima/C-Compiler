#include "9cc.h"

void Token::tokenize()
{
    char *p = user_input;

    Token head(user_input);
    head.mNext = nullptr;
    Token *cur = &head;

    while (*p)
    {
        if (isspace(*p))
        {
            p++;
            continue;
        }

        if (!memcmp(p, "==", 2) ||
            !memcmp(p, "!=", 2) ||
            !memcmp(p, "<=", 2) ||
            !memcmp(p, ">=", 2))
        {
            cur = new_token(TokenKind::TK_RESERVED, cur, p, 2);
            p += 2;
            continue;
        }

        if ('a' <= *p && *p <= 'z')
        {
            cur = new_token(TokenKind::TK_IDENT, cur, p, 1);
            p++;
            continue;
        }

        if (strchr("+-*/()<>=;", *p))
        {
            cur = new_token(TokenKind::TK_RESERVED, cur, p, 1);
            p++;
            continue;
        }

        if (isdigit(*p))
        {
            cur = new_token(TokenKind::TK_NUM, cur, p, 0);
            cur->val = strtol(p, &p, 10);
            continue;
        }

        error_at(p, "トークナイズ出来ませんでした。");
    }

    new_token(TokenKind::TK_EOF, cur, p, 0);
    token = head.mNext;
}

Token *Token::new_token(TokenKind kind, Token *cur, char *str, int len)
{
    Token *token = reinterpret_cast<Token *>(calloc(1, sizeof(Token)));
    token->mKind = kind;
    token->str = str;
    token->len = len;
    token->user_input = cur->user_input;
    cur->mNext = token;
    return token;
}

bool Token::consume(const char *op)
{
    if (token->mKind != TokenKind::TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
    {
        return false;
    }

    token = token->mNext;
    return true;
}

void Token::expect(const char *op)
{
    if (token->mKind != TokenKind::TK_RESERVED ||
        strlen(op) != token->len ||
        memcmp(token->str, op, token->len))
    {
        error_at(token->str, "'%c'ではありません。", *op);
    }

    token = token->mNext;
}

int Token::expect_number()
{
    if (token->mKind != TokenKind::TK_NUM)
    {
        error_at(token->str, "数ではありません。");
    }

    int val = token->val;
    token = token->mNext;
    return val;
}


bool Token::is_lvar(int& offset)
{
    if (token->mKind != TokenKind::TK_IDENT)
    {
        return false;
    }


    offset = (token->str[0] - 'a' + 1) * LOCAL_VAR_SIZE;
    token = token->mNext;
    return true;
}

bool Token::at_end() const
{
    return token->mKind == TokenKind::TK_EOF;
}


void Token::error_at(const char *location, const char *fmt, ...)
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