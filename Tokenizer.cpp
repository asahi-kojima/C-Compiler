#include "Tokenizer.h"

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

        error_at(p, "トークナイズ出来ませんでした。");
    }

    new_token(TokenKind::TK_EOF, cur, p);
    token =  head.mNext;
}

Token *Token::new_token(TokenKind kind, Token *cur, char *str)
{
    Token *token = reinterpret_cast<Token *>(calloc(1, sizeof(Token)));
    token->mKind = kind;
    token->str = str;
    token->user_input  = cur->user_input;
    cur->mNext = token;
    return token;
}

bool Token::consume(char op)
{
    if (token->mKind != TokenKind::TK_RESERVED || token->str[0] != op)
    {
        return false;
    }

    token = token->mNext;
    return true;
}

void Token::expect(char op)
{
    if (token->mKind != TokenKind::TK_RESERVED || token->str[0] != op)
    {
        error_at(token->str, "'%c'ではありません。", op);
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

bool Token::at_end() const
{
    return token->mKind == TokenKind::TK_EOF;
}


