#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "tokenizer.h"


namespace
{
    void error_at(const char *location, const char *user_input, const char *fmt, ...)
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

    bool is_identifier_char(char c)
    {
        return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || ('0' <= c && c <= '9') || (c == '_');
    }
}


TokenStream::TokenStream(char* input_string)
: m_input_string(input_string)
{
    tokenize();
    m_current_token_iter = m_token_list.begin();
    m_end_token_iter = m_token_list.end();
}


void TokenStream::print_all_tokens() const
{
    for (auto iter = m_token_list.begin(), end = m_token_list.end(); iter->token_kind != TokenKind::TK_EOF; iter++)
    {
        const u32 pos = m_token_list.size()  - (end - iter);
        const Token& token = *iter;

        fprintf(stderr, "[%d] : ", pos);

        switch (token.token_kind)
        {
        case TokenKind::TK_NUM:
            fprintf(stderr, "%d\n", iter->property.of_num.value);
            break;
        
        case TokenKind::TK_RESERVED:
        case TokenKind::TK_IDENT:
            char reserved_string[100];
            strncpy(reserved_string, iter->property.of_string.str, iter->property.of_string.len);
            reserved_string[iter->property.of_string.len] = '\0';
            fprintf(stderr, "%s\n", reserved_string);
            break;

        default:
            fprintf(stderr, "can't reach here : error in switch statement\n");
            exit(1);
            break;
        }
    }
}  



void TokenStream::tokenize()
{
    char* p = m_input_string;
    
    while (*p)
    {
        //空白なら飛ばす
        if (isspace(*p))
        {
            p++;
            continue;
        }

        //"return"を検知する
        if (!strncmp(p, "return", 6) && !is_identifier_char(*(p + 6)))
        {
            Token::TokenProperty property;
            {
                property.of_string.str = p;
                property.of_string.len = 6;
            }

            m_token_list.emplace_back(TokenKind::TK_RESERVED, p, property);
            p += 6;
            continue;
        }

        
        if (!memcmp(p, "==", 2) || !memcmp(p, "!=", 2) || !memcmp(p, "<=", 2) || !memcmp(p, ">=", 2))
        {
            Token::TokenProperty property;
            {
                property.of_string.str = p;
                property.of_string.len = 2;
            }

            m_token_list.emplace_back(TokenKind::TK_RESERVED, p, property);
            p+=2;
            continue;
        }

        if (strchr("+-*/()<>=;{}", *p))
        {
            Token::TokenProperty property;
            {
                property.of_string.str = p;
                property.of_string.len = 1;
            }

            m_token_list.emplace_back(TokenKind::TK_RESERVED, p, property);
            p++;
            continue;
        }

        if ('a' <= *p && *p <= 'z')
        {
            u32 len = 1;
            while (true)
            {
                const char next_char = *(p + len);
                if (is_identifier_char(next_char))
                {
                    len++;
                }
                else
                {
                    break;
                }
            }

            Token::TokenProperty property;
            {
                property.of_string.str = p;
                property.of_string.len = len;
            }

            m_token_list.emplace_back(TokenKind::TK_IDENT, p, property);
            p += len;
            continue;
        }


        if (isdigit(*p))
        {
            const char* token_position_in_input_string = p;
            Token::TokenProperty property;
            {
                property.of_num.value = strtol(p, &p, 10); 
            }

            m_token_list.emplace_back(TokenKind::TK_NUM, token_position_in_input_string, property);
            continue;
        }

        fprintf(stderr, "不明な文字 '%c' が入力されました。\n", *p);
        exit(1); //ここに来るのは想定外の文字列なので、エラー終了する。
    }

    //これは重要(ベクターで管理しているから末尾はいらないと思ったが、番兵がいないとパーサーが一個先を読むとかが出来なくなる)。
    m_token_list.emplace_back(TokenKind::TK_EOF, p);
}


bool TokenStream::consume_if(const char* str, TokenKind kind)
{
    const Token& current_token = *m_current_token_iter;
    
    if (current_token.token_kind != kind ||
        static_cast<size_t>(current_token.property.of_string.len) != strlen(str) ||
        memcmp(current_token.property.of_string.str, str, current_token.property.of_string.len))
    {
        return false;
    }

    m_current_token_iter++;
    return true;
}

void TokenStream::expect(const char* str)
{
    const Token& current_token = *m_current_token_iter;
    if (current_token.token_kind != TokenKind::TK_RESERVED ||
        static_cast<size_t>(current_token.property.of_string.len) != strlen(str) ||
        current_token.property.of_string.str[0] != str[0])
    {
        error_at(current_token.token_position_in_input_string, m_input_string, "'%c'が期待されますが、'%c'が確認されました。", *str, current_token.token_position_in_input_string[0]);
    }

    m_current_token_iter++;
}

s32 TokenStream::expect_number()
{
    const Token& current_token = *m_current_token_iter;

    if (current_token.token_kind != TokenKind::TK_NUM)
    {
        error_at(current_token.token_position_in_input_string, m_input_string, "数ではありません。");
    }

    const s32 value =current_token.property.of_num.value;

    m_current_token_iter++;
    return value;
}

std::string TokenStream::expect_ident()
{
    if (m_current_token_iter->token_kind != TokenKind::TK_IDENT)
    {
        error_at(m_current_token_iter->token_position_in_input_string, m_input_string, "識別子ではありません。");
    }

    const auto& [str, len] = m_current_token_iter->property.of_string;
    m_current_token_iter++;
    return std::string(str, len);
}

void TokenStream::skip()
{
    m_current_token_iter++;
}

Token* TokenStream::get_current_token_ptr() const
{
    return &(*m_current_token_iter);
}

bool TokenStream::at_end() const
{
    return m_current_token_iter->token_kind == TokenKind::TK_EOF;
}
