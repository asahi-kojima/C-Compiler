#include "9cc.h"

Node* code[100];

Node *new_node(NodeKind kind, Node *lhs, Node *rhs)
{
    Node *node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *new_node_num(int num)
{
    Node *node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
    node->kind = NodeKind::ND_NUM;
    node->val = num;
    return node;
}

Node *new_node_lvar(int offset)
{
    Node *node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
    node->kind = NodeKind::ND_LVAR;
    node->offset = offset;
    return node;
}



void program(Token& token);
Node* statement(Token& token);
Node* expr(Token& token);
Node* assign(Token& token);
Node *equality(Token &token);
Node *relatinal(Token &token);
Node *add(Token &token);
Node *mul(Token &token);
Node *unary(Token &);
Node *primary(Token &);

void program(Token& token)
{
    int i = 0;
    while(!token.at_end())
    {
        code[i] = statement(token);
        i++;
    }

    code[i] = nullptr;
}

Node* statement(Token& token)
{
    Node* node = expr(token);
    token.expect(";");
    return node;
}


Node *expr(Token &token)
{
    Node *node = assign(token);
    return node;
}


Node *assign(Token &token)
{
    Node *node = equality(token);
    if (token.consume("="))
    {
        node = new_node(NodeKind::ND_ASSIGN, node, assign(token));
    }

    return node;
}

Node *equality(Token &token)
{
    Node *node = relatinal(token);

    for (;;)
    {
        if (token.consume("=="))
        {
            node = new_node(NodeKind::ND_EQ, node, relatinal(token));
        }
        else if (token.consume("!="))
        {
            node = new_node(NodeKind::ND_NE, node, relatinal(token));
        }
        else
        {
            return node;
        }
    }
}

Node *relatinal(Token &token)
{
    Node *node = add(token);

    for (;;)
    {
        if (token.consume("<"))
        {
            node = new_node(NodeKind::ND_LT, node, add(token));
        }
        else if (token.consume("<="))
        {
            node = new_node(NodeKind::ND_LE, node, add(token));
        }
        if (token.consume(">"))
        {
            node = new_node(NodeKind::ND_LT, add(token), node);
        }
        else if (token.consume(">="))
        {
            node = new_node(NodeKind::ND_LE, add(token), node);
        }
        else
        {
            return node;
        }
    }
}

Node *add(Token &token)
{
    Node *node = mul(token);

    for (;;)
    {
        if (token.consume("+"))
        {
            node = new_node(NodeKind::ND_ADD, node, mul(token));
        }
        else if (token.consume("-"))
        {
            node = new_node(NodeKind::ND_SUB, node, mul(token));
        }
        else
        {
            return node;
        }
    }
}

Node *mul(Token &token)
{
    Node *node = unary(token);

    for (;;)
    {
        if (token.consume("*"))
        {
            node = new_node(NodeKind::ND_MUL, node, unary(token));
        }
        else if (token.consume("/"))
        {
            node = new_node(NodeKind::ND_DIV, node, unary(token));
        }
        else
        {
            return node;
        }
    }
}

Node *unary(Token &token)
{
    if (token.consume("+"))
    {
        return primary(token);
    }
    if (token.consume("-"))
    {
        return new_node(NodeKind::ND_SUB, new_node_num(0), primary(token));
    }
    return primary(token);
}

Node *primary(Token &token)
{
    if (token.consume("("))
    {
        Node *node = expr(token);
        token.expect(")");
        return node;
    }

    if (int offset = -1; token.is_lvar(offset))
    {
        return new_node_lvar(offset);
    }

    return new_node_num(token.expect_number());
}
