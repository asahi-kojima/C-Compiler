#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <algorithm>
#include "parser.h"

namespace
{
    AstNode* make_new_node(AstNodeKind kind, AstNode* lhs_node, AstNode* rhs_node, AstNode* condition_node = nullptr)
    {
        AstNode* node = reinterpret_cast<AstNode*>(calloc(1, sizeof(AstNode)));
        node->kind =kind;
        node->lhs_node = lhs_node;
        node->rhs_node = rhs_node;
        node->condition_node = condition_node;

        return node;
    }

    AstNode* make_new_node_of_ident(const char* name_of_ident, u32 len, u32 offset)
    {
        AstNode* node = make_new_node(AstNodeKind::ND_LVAR, nullptr, nullptr);
        node->property.of_ident.name = name_of_ident;
        node->property.of_ident.len = len;
        node->property.of_ident.offset = offset;

        return node;
    }

    AstNode* make_new_node_of_function_call(const char* name_of_ident, u32 len)
    {
        AstNode* node = make_new_node(AstNodeKind::ND_FUNCTION_CALL, nullptr, nullptr);
        node->property.of_ident.name = name_of_ident;
        node->property.of_ident.len = len;

        return node;
    }


    AstNode* make_new_node_of_num(s32 value)
    {
        AstNode* node = make_new_node(AstNodeKind::ND_NUM, nullptr, nullptr);
        node->property.of_num.value = value;

        return node;
    }

    // ASTを再帰的に表示するためのヘルパー関数
    void print_ast_recursive(AstNode* node, std::string prefix, bool is_last) {
        if (!node) {
            return;
        }

        // ノードの接続線を表示
        fprintf(stderr, "%s", prefix.c_str());
        fprintf(stderr, "%s", is_last ? "└── " : "├── ");

        // ノードの種類と値を表示
        switch (node->kind) {
            case AstNodeKind::ND_ADD:     fprintf(stderr, "ND_ADD\n"); break;
            case AstNodeKind::ND_SUB:     fprintf(stderr, "ND_SUB\n"); break;
            case AstNodeKind::ND_MUL:     fprintf(stderr, "ND_MUL\n"); break;
            case AstNodeKind::ND_DIV:     fprintf(stderr, "ND_DIV\n"); break;
            case AstNodeKind::ND_EQ:      fprintf(stderr, "ND_EQ\n"); break;
            case AstNodeKind::ND_NE:      fprintf(stderr, "ND_NE\n"); break;
            case AstNodeKind::ND_LT:      fprintf(stderr, "ND_LT\n"); break;
            case AstNodeKind::ND_LE:      fprintf(stderr, "ND_LE\n"); break;
            case AstNodeKind::ND_RETURN:  fprintf(stderr, "ND_RETURN\n"); break;
            case AstNodeKind::ND_IF:      fprintf(stderr, "ND_IF\n"); break;
            case AstNodeKind::ND_WHILE:   fprintf(stderr, "ND_WHILE\n"); break;
            case AstNodeKind::ND_FOR:   fprintf(stderr, "ND_FOR\n"); break;
            case AstNodeKind::ND_BLOCK:   fprintf(stderr, "ND_BLOCK\n"); break;
            case AstNodeKind::ND_ASSIGN:  fprintf(stderr, "ND_ASSIGN\n"); break;
            case AstNodeKind::ND_LVAR:
                fprintf(stderr, "ND_LVAR: %.*s\n", node->property.of_ident.len, node->property.of_ident.name);
                break;
            case AstNodeKind::ND_NUM:
                fprintf(stderr, "ND_NUM: %d\n", node->property.of_num.value);
                break;
        }

        // 子ノードへのプレフィックスを計算
        std::string child_prefix = prefix + (is_last ? "    " : "│   ");

        // ND_IFとND_WHILEはcondition_nodeを持つ
        if (node->kind == AstNodeKind::ND_IF || node->kind == AstNodeKind::ND_WHILE) {
            // condition_node, lhs_node, rhs_nodeの順で表示
            print_ast_recursive(node->condition_node, child_prefix, false);
            print_ast_recursive(node->lhs_node, child_prefix, node->rhs_node == nullptr);
            print_ast_recursive(node->rhs_node, child_prefix, true);
        } else {
            // rhsが存在する場合、lhsは最後の子ではない
            print_ast_recursive(node->lhs_node, child_prefix, node->rhs_node == nullptr);
            print_ast_recursive(node->rhs_node, child_prefix, true);
        }
    }
}


std::vector<FunctionRecord> Parser::program()
{
    std::vector<FunctionRecord> function_to_nodes_map;
    while (!m_token_stream_ptr->at_end())
    {
        FunctionRecord record = function();
        function_to_nodes_map.push_back(record);
    }
    return function_to_nodes_map;
}

namespace
{   
    std::vector<std::string>* current_local_variable_names = nullptr;
}

FunctionRecord Parser::function()
{
    std::vector<AstNode*> nodes;
    std::vector<std::string> local_variable_names;
    current_local_variable_names = &local_variable_names;

    // 関数名の確認
    const std::string function_name = m_token_stream_ptr->expect_ident();
    m_token_stream_ptr->expect("(");
    m_token_stream_ptr->expect(")");
    m_token_stream_ptr->expect("{");
    while (!m_token_stream_ptr->consume_if("}"))
    {
        // 関数の中身を解析
        AstNode* node = statement();
        if (node)
        {
            nodes.push_back(node);
        }
        else
        {
            fprintf(stderr, "構文エラー: ステートメントが期待されました。\n");
            exit(1);
        }
    }

    FunctionRecord record(function_name, nodes, local_variable_names.size() * LOCAL_VARIABLE_BYTE_SIZE);

    return record;
}

AstNode* Parser::statement()
{
    if (m_token_stream_ptr->consume_if("return", TokenKind::TK_RETURN))
    {
        AstNode* node = expr();
        m_token_stream_ptr->expect(";");
        node = make_new_node(AstNodeKind::ND_RETURN, node, nullptr);
        return node;
    }

    if (m_token_stream_ptr->consume_if("if", TokenKind::TK_IF))
    {
        m_token_stream_ptr->expect("(");
        AstNode* condition = expr();
        m_token_stream_ptr->expect(")");
        AstNode* then_node = statement();

        // elseがあるか確認
        AstNode* else_node = nullptr;
        if (m_token_stream_ptr->consume_if("else", TokenKind::TK_ELSE))
        {
            else_node = statement();
        }

        return make_new_node(AstNodeKind::ND_IF, then_node, else_node, condition);
    }

    if (m_token_stream_ptr->consume_if("while", TokenKind::TK_WHILE))
    {
        m_token_stream_ptr->expect("(");
        AstNode* condition = expr();
        m_token_stream_ptr->expect(")");
        AstNode* statement_in_while = statement();

        return make_new_node(AstNodeKind::ND_WHILE, statement_in_while, nullptr, condition);
    }

    if (m_token_stream_ptr->consume_if("for", TokenKind::TK_FOR))
    {
        m_token_stream_ptr->expect("(");
        AstNode* init = nullptr;
        AstNode* condition = nullptr;
        AstNode* update = nullptr;

        // 初期化式があるか確認
        if (!m_token_stream_ptr->consume_if(";"))
        {
            init = expr();
            m_token_stream_ptr->expect(";");
        }
        // 条件式があるか確認
        if (!m_token_stream_ptr->consume_if(";"))
        {
            condition = expr();
            m_token_stream_ptr->expect(";");
        }
        // 更新式があるか確認
        if (!m_token_stream_ptr->consume_if(")"))
        {
            update = expr();
            m_token_stream_ptr->expect(")");
        }

        AstNode* body = statement();

        AstNode* for_node = reinterpret_cast<AstNode*>(calloc(1, sizeof(AstNode)));
        {
            for_node->kind =AstNodeKind::ND_FOR;
            for_node->lhs_node = nullptr;
            for_node->rhs_node = nullptr;
            for_node->property.of_for.init = init;
            for_node->property.of_for.cond = condition;
            for_node->property.of_for.update = update;
            for_node->property.of_for.body = body;
        }

        return for_node;
    }

    if (m_token_stream_ptr->consume_if("{", TokenKind::TK_RESERVED))
    {
        std::vector<AstNode*> node_vec;
        while (!m_token_stream_ptr->consume_if("}", TokenKind::TK_RESERVED))
        {
            AstNode* node = statement();
            if (!node)
            {
                fprintf(stderr, "nullptr\n");
                exit(1);
            }
            node_vec.push_back(node);
        }

        const u32 block_size = node_vec.size();
        AstNode** block_nodes = reinterpret_cast<AstNode**>(malloc(sizeof(AstNode*) * block_size));
        for (u32 i = 0; i < block_size; i++)
        {
            block_nodes[i] = node_vec[i];
        }

        AstNode* node = reinterpret_cast<AstNode*>(calloc(1, sizeof(AstNode)));
        {
            node->kind =AstNodeKind::ND_BLOCK;
            node->lhs_node = nullptr;
            node->rhs_node = nullptr;
            node->property.of_block.block_nodes = block_nodes;
            node->property.of_block.block_size = block_size;
        }

        return node;
    }

    AstNode* node = expr();
    m_token_stream_ptr->expect(";");
    return node;
}

AstNode* Parser::expr()
{
    return assign();
}

AstNode* Parser::assign()
{
    AstNode* node = equality();
    if (m_token_stream_ptr->consume_if("="))
    {
        node = make_new_node(AstNodeKind::ND_ASSIGN, node, assign());
    }
    
    return node;
}

AstNode* Parser::equality()
{
    AstNode* node = relational();
    while(true)
    {
        if (m_token_stream_ptr->consume_if("=="))
        {
            node = make_new_node(AstNodeKind::ND_EQ, node, relational());
        }
        else if (m_token_stream_ptr->consume_if("!="))
        {
            node = make_new_node(AstNodeKind::ND_NE, node, relational());
        }
        else
        {
            return node;
        }
    }
}

AstNode* Parser::relational()
{
    AstNode* node = add();
    while(true)
    {
        if (m_token_stream_ptr->consume_if("<"))
        {
            node = make_new_node(AstNodeKind::ND_LT, node, add());
        }
        else if (m_token_stream_ptr->consume_if("<="))
        {
            node = make_new_node(AstNodeKind::ND_LE, node, add());
        }
        else if (m_token_stream_ptr->consume_if(">"))
        {
            node = make_new_node(AstNodeKind::ND_LT, add(), node);
        }
        else if (m_token_stream_ptr->consume_if(">="))
        {
            node = make_new_node(AstNodeKind::ND_LE, add(), node); // a >= b は b <= a と同じ
        }
        else
        {
            return node;
        }
    }
}

AstNode* Parser::add()
{
    AstNode* node = mul();
    while(true)
    {
        if (m_token_stream_ptr->consume_if("+"))
        {
            node = make_new_node(AstNodeKind::ND_ADD, node, mul());
        }
        else if (m_token_stream_ptr->consume_if("-"))
        {
            node = make_new_node(AstNodeKind::ND_SUB, node, mul());
        }
        else
        {
            return node;
        }
    }
}

AstNode* Parser::mul()
{
    AstNode* node = unary();
    while(true)
    {
        if (m_token_stream_ptr->consume_if("*"))
        {
            node = make_new_node(AstNodeKind::ND_MUL, node, unary());
        }
        else if (m_token_stream_ptr->consume_if("/"))
        {
            node = make_new_node(AstNodeKind::ND_DIV, node, unary());
        }
        else
        {
            return node;
        }
    }
}

AstNode* Parser::unary()
{
    if (m_token_stream_ptr->consume_if("+"))
    {
        return primary();
    }
    else if (m_token_stream_ptr->consume_if("-"))
    {
        return make_new_node(AstNodeKind::ND_SUB, make_new_node_of_num(0), primary());
    }
    else
    {
        return primary();
    }
}

AstNode* Parser::primary()
{
    if (m_token_stream_ptr->consume_if("("))
    {
        AstNode* node = expr();
        m_token_stream_ptr->expect(")");
        return node;
    }

    if (m_token_stream_ptr->get_current_token_ptr()->token_kind == TokenKind::TK_IDENT)
    {
        const auto& [str, len] = m_token_stream_ptr->get_current_token_ptr()->property.of_string;
        std::string ident_name(str, len);
        m_token_stream_ptr->skip();

        //識別子の後に括弧が来ていれば関数呼び出し
        if (m_token_stream_ptr->consume_if("(", TokenKind::TK_RESERVED))
        {
            m_token_stream_ptr->expect(")");
            return make_new_node_of_function_call(str, len);
        }

        //ローカル変数のテーブル内に既に存在しているか確認し、なければ新規に追加する。
        auto it = std::find(current_local_variable_names->begin(), current_local_variable_names->end(), ident_name);
        if (it == current_local_variable_names->end()) {
            current_local_variable_names->push_back(ident_name);
        }
        const u32 offset = std::distance(current_local_variable_names->begin(), std::find(current_local_variable_names->begin(), current_local_variable_names->end(), ident_name)) + 1;
#ifdef DEBUG
        fprintf(stderr, "識別子 '%s' のオフセット: %d\n", ident_name.c_str(), offset);//delete this line in production code
#endif
        return make_new_node_of_ident(str, len,offset);
    }

    return make_new_node_of_num(m_token_stream_ptr->expect_number());
}


void print_ast(AstNode* node)
{
    print_ast_recursive(node, "", true);
}
