#include "9cc.h"

namespace
{
    bool is_part_of_string(const char c)
    {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || (c == '_');
    }
}


TokenList::TokenList(char *user_input)
    : current_token(nullptr),
      user_input(user_input)
{
    tokenize();
}

void TokenList::tokenize()
{
    char *p = user_input;

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

        if (strncmp(p, "return", 6) == 0 && !is_part_of_string(p[6]))
        {
            cur = new_token(TokenKind::TK_RETURN, cur, p, 6);
            p += 6;
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
            int len = 1;
            while (true)
            {
                const char next_char = *(p + len);
                if ('a' <= next_char && next_char <= 'z')
                {
                    len++;
                }
                else
                {
                    break;
                }
            }
            cur = new_token(TokenKind::TK_IDENT, cur, p, len);
            p += len;
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
    current_token = head.mNext;
}

Token *TokenList::new_token(TokenKind kind, Token *cur, const char *str, int len)
{
    Token *token = reinterpret_cast<Token *>(calloc(1, sizeof(Token)));
    token->mKind = kind;
    token->str = str;
    token->len = len;
    cur->mNext = token;
    return token;
}

bool TokenList::consume(const char *op)
{
    if (current_token->mKind != TokenKind::TK_RESERVED ||
        strlen(op) != current_token->len ||
        memcmp(current_token->str, op, current_token->len))
    {
        return false;
    }

    current_token = current_token->mNext;
    return true;
}
bool TokenList::consume_if_return()
{
    if (current_token->mKind != TokenKind::TK_RETURN)
    {
        return false;
    }

    current_token = current_token->mNext;
    return true;
}

void TokenList::expect(const char *op)
{
    if (current_token->mKind != TokenKind::TK_RESERVED ||
        strlen(op) != current_token->len ||
        memcmp(current_token->str, op, current_token->len))
    {
        error_at(current_token->str, "'%c'が期待されますが、'%c'が確認されました。", *op, *current_token->str);
    }

    current_token = current_token->mNext;
}

int TokenList::expect_number()
{
    if (current_token->mKind != TokenKind::TK_NUM)
    {
        error_at(current_token->str, "数ではありません。");
    }

    int val = current_token->val;
    current_token = current_token->mNext;
    return val;
}

Token *TokenList::expect_lvar()
{
    if (current_token->mKind != TokenKind::TK_IDENT)
    {
        error_at(current_token->str, "変数ではありません。");
    }
    Token *cur = current_token;
    current_token = current_token->mNext;
    return cur;
}

bool TokenList::at_end() const
{
    return (current_token->mKind == TokenKind::TK_EOF);
}

void TokenList::error_at(const char *location, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    int pos = location - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}