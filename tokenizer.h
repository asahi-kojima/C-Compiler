#pragma once
#include "essential.h"
#include <vector>

enum class TokenKind
{
    TK_RESERVED,
    TK_IF,
    TK_WHILE,
    TK_FOR,
    TK_RETURN,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
};


struct Token
{
    TokenKind token_kind;

    const char* token_position_in_input_string;

    union TokenProperty
    {
        struct OfNum {s32 value;} of_num;
        struct OfString {const char* str;  s32 len;} of_string;
    } property;

    Token(TokenKind kind, const char* token_position_in_input_string, TokenProperty property)
    : token_kind(kind)
    , token_position_in_input_string(token_position_in_input_string)
    , property(property){}
};

class TokenStream
{
public:
    TokenStream(char* input_string);

    bool consume_if(const char* str);
    bool consume_if(TokenKind kind);
    void expect(const char* str);
    s32 expect_number();

    Token get_current_token() const;
    bool at_end() const;

    void print_all_tokens() const;


private:
    void tokenize();

    std::vector<Token> m_token_list;
    //u32 m_current_token_index;

    std::vector<Token>::iterator m_current_token_iter;
    std::vector<Token>::iterator m_end_token_iter;

    char* m_input_string = nullptr;

};