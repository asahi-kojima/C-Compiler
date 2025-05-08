#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"


AstNode* make_new_node(AstNodeKind kind, AstNode* lhs_node, AstNode* rhs_node)
{
    AstNode* node = reinterpret_cast<AstNode*>(calloc(1, sizeof(AstNode)));
    node->kind =kind;
    node->lhs_node = lhs_node;
    node->rhs_node = rhs_node;

    return node;
}

AstNode* make_new_node_of_num(s32 value)
{
    AstNode* node = make_new_node(AstNodeKind::ND_NUM, nullptr, nullptr);
    node->property.of_num.value = value;

    return node;
}


AstNode* Parser::expr()
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

    return make_new_node_of_num(m_token_stream_ptr->expect_number());
}
