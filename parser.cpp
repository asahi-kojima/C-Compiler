#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include "parser.h"

namespace
{
    AstNode* make_new_node(AstNodeKind kind, AstNode* lhs_node, AstNode* rhs_node)
    {
        AstNode* node = reinterpret_cast<AstNode*>(calloc(1, sizeof(AstNode)));
        node->kind =kind;
        node->lhs_node = lhs_node;
        node->rhs_node = rhs_node;

        return node;
    }

    AstNode* make_new_node_of_ident(const char* name_of_ident)
    {
        AstNode* node = make_new_node(AstNodeKind::ND_LVAR, nullptr, nullptr);
        node->property.of_ident.name = name_of_ident;

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
            case AstNodeKind::ND_ASSIGN:  fprintf(stderr, "ND_ASSIGN\n"); break;
            case AstNodeKind::ND_LVAR:
                // 変数名は1文字と仮定
                fprintf(stderr, "ND_LVAR: %.*s\n", 1, node->property.of_ident.name);
                break;
            case AstNodeKind::ND_NUM:
                fprintf(stderr, "ND_NUM: %d\n", node->property.of_num.value);
                break;
        }

        // 子ノードへのプレフィックスを計算
        std::string child_prefix = prefix + (is_last ? "    " : "│   ");

        // 子ノードを再帰的に表示
        // rhsが存在する場合、lhsは最後の子ではない
        print_ast_recursive(node->lhs_node, child_prefix, node->rhs_node == nullptr);
        print_ast_recursive(node->rhs_node, child_prefix, true);
    }
}


std::vector<AstNode*> Parser::program()
{
    std::vector<AstNode*> nodes;
    while (!m_token_stream_ptr->at_end())
    {
        nodes.push_back(statement());
    }
    return nodes;
}

AstNode* Parser::statement()
{
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
        m_token_stream_ptr->skip();
        return make_new_node_of_ident(str);
    }

    return make_new_node_of_num(m_token_stream_ptr->expect_number());
}


void print_ast(AstNode* node)
{
    print_ast_recursive(node, "", true);
}
