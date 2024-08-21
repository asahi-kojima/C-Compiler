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

        
        if (const int len_of_if = 2; strncmp(p, "if", len_of_if) == 0 && !is_part_of_string(p[len_of_if]))
        {
            cur = new_token(TokenKind::TK_IF, cur, p, len_of_if);
            p += len_of_if;
            continue;
        }

       
        if (const int len_of_while = 5; strncmp(p, "while", len_of_while) == 0 && !is_part_of_string(p[len_of_while]))
        {
            cur = new_token(TokenKind::TK_WHILE, cur, p, len_of_while);
            p += len_of_while;
            continue;
        }

        
        if (const int len_of_for = 3; strncmp(p, "for", len_of_for) == 0 && !is_part_of_string(p[len_of_for]))
        {
            cur = new_token(TokenKind::TK_FOR, cur, p, len_of_for);
            p += len_of_for;
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

        if (('a' <= *p && *p <= 'z') || ('A' <= *p && *p <= 'Z'))
        {
            int len = 1;
            while (true)
            {
                const char next_char = *(p + len);
                if (is_part_of_string(next_char))
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

        if (strchr("+-*/()<>=;{},", *p))
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

        error_at(p, "%cをトークナイズ出来ませんでした。", *p);
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

bool TokenList::consume_if(TokenKind kind)
{
    if (current_token->mKind != kind)
    {
        return false;
    }

    current_token = current_token->mNext;
    return true;
}

bool TokenList::consume_if(const char* token)
{
    if (current_token->len != strlen(token) || memcmp(current_token->str, token, current_token->len))
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
    user_input[pos + 1] = '\0';
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, "");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}