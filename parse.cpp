#include "9cc.h"

LVar *locals = nullptr;
Node *code[100];

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

LVar *find_lvar(const Token &token)
{
    for (LVar *var = locals; var; var = var->next)
    {
        if (var->len == token.len && !memcmp(token.str, var->name, var->len))
        {
            return var;
        }
    }

    return nullptr;
}

void make_new_lvar(const Token &token)
{
    LVar *new_lvar = reinterpret_cast<LVar*>(calloc(1, sizeof(LVar)));
    new_lvar->len = token.len; 
    new_lvar->name = token.str;

    LVar *lvar = locals;
    if (lvar)
    {
        for (; lvar->next; lvar = lvar->next)
        {
        }
        new_lvar->offset = lvar->offset + LOCAL_VAR_SIZE;
        lvar->next = new_lvar;
    }
    else
    {
        new_lvar->offset = 1 * LOCAL_VAR_SIZE;
        locals = new_lvar;
    }
}

void program(TokenList &token);
Node *statement(TokenList &token);
Node *expr(TokenList &token);
Node *assign(TokenList &token);
Node *equality(TokenList &token);
Node *relatinal(TokenList &token);
Node *add(TokenList &token);
Node *mul(TokenList &token);
Node *unary(TokenList &);
Node *primary(TokenList &);

void program(TokenList &token)
{
    int i = 0;
    while (!token.at_end())
    {
        code[i] = statement(token);
        i++;
    }

    code[i] = nullptr;
}

Node *statement(TokenList &token)
{
    Node *node = expr(token);
    token.expect(";");
    return node;
}

Node *expr(TokenList &token)
{
    Node *node = assign(token);
    return node;
}

Node *assign(TokenList &token)
{
    Node *node = equality(token);
    if (token.consume("="))
    {
        node = new_node(NodeKind::ND_ASSIGN, node, assign(token));
    }

    return node;
}

Node *equality(TokenList &token)
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

Node *relatinal(TokenList &token)
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

Node *add(TokenList &token)
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

Node *mul(TokenList &token)
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

Node *unary(TokenList &token)
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

Node *primary(TokenList &token)
{
    if (token.consume("("))
    {
        Node *node = expr(token);
        token.expect(")");
        return node;
    }

    switch (token.getCurrentToken()->mKind)
    {
    case TokenKind::TK_IDENT:
    {
        const Token& lvar_token = *token.expect_lvar();
        LVar *lvar = find_lvar(lvar_token);
        if (lvar)
        {
            return new_node_lvar(lvar->offset);
        }
        else
        {
            make_new_lvar(lvar_token);
            LVar *lvar = find_lvar(lvar_token);
            return new_node_lvar(lvar->offset);
        }
        break;
    }

    case TokenKind::TK_NUM:
        return new_node_num(token.expect_number());
        break;
    
    default:
        error("ここに到達することは構文違反です。\n");
        exit(1);
    }

    
}
