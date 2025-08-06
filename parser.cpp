#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
            node = make_new_node(AstNodeKind::ND_LE, add(), node);
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
