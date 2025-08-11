#pragma once

#include <map>
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

    ND_ASSIGN,
    ND_LVAR,

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
        struct {const char* name;} of_ident;
    } property;
};

class FunctionRecord;
class Parser
{
public:
    Parser(TokenStream* token_stream_ptr)
    : m_token_stream_ptr(token_stream_ptr){}

    std::map<std::string, std::vector<AstNode*>> program();
    FunctionRecord function();
    AstNode* statement();
    AstNode* expr();
    AstNode* assign();
    AstNode* equality();
    AstNode* relational();
    AstNode* add();
    AstNode* mul();
    AstNode* unary();
    AstNode* primary();

private:
    TokenStream* m_token_stream_ptr = nullptr;
};

// ASTをツリー形式で標準エラー出力に表示する (デバッグ用)
void print_ast(AstNode* node);