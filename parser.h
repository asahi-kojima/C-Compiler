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

class FunctionRecord
{
public:
    FunctionRecord(const std::string& name, const std::vector<AstNode*>& nodes, u32 stack_size)
    : name(name), nodes(nodes), stack_size(stack_size) {}

    // 関数名を取得
    const std::string& get_name() const { return name; }

    // ノードのリストを取得
    const std::vector<AstNode*>& get_nodes() const { return nodes; }

    // スタックサイズを取得
    u32 get_stack_size() const { return stack_size; }

    // スタックサイズを設定
    void set_stack_size(u32 size) { stack_size = size; }
    
private:
    std::string name;
    std::vector<AstNode*> nodes;
    u32 stack_size;
};

class Parser
{
public:
    Parser(TokenStream* token_stream_ptr)
    : m_token_stream_ptr(token_stream_ptr){}

    std::vector<FunctionRecord> program();
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