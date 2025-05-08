#pragma once

#include "essential.h"
#include "tokenizer.h"

enum class AstNodeKind
{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,

    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,

    ND_NUM,
};


struct AstNode
{
    AstNodeKind kind;
    AstNode* lhs_node;
    AstNode* rhs_node;

    union 
    {
        struct {s32 value;} of_num;
    } property;
};

class Parser
{
public:
    Parser(TokenStream* token_stream_ptr)
    : m_token_stream_ptr(token_stream_ptr){}

    AstNode* expr();
    AstNode* equality();
    AstNode* relational();
    AstNode* add();
    AstNode* mul();
    AstNode* unary();
    AstNode* primary();

private:
    TokenStream* m_token_stream_ptr = nullptr;
};