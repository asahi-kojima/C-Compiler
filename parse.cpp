#include "9cc.h"

LVar *locals = nullptr;
Node *code[100];

Node *new_node(NodeKind kind, Node *lhs, Node *rhs, Node *ths = nullptr, Node *fhs = nullptr)
{
    Node *node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
    node->kind = kind;
    node->lhs = lhs;
    node->rhs = rhs;
    node->ths = ths;
    node->fhs = fhs;
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
    LVar *new_lvar = reinterpret_cast<LVar *>(calloc(1, sizeof(LVar)));
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

void program(TokenList &token_list);
Node *statement(TokenList &token_list);
Node *expr(TokenList &token_list);
Node *assign(TokenList &token_list);
Node *equality(TokenList &token_list);
Node *relatinal(TokenList &token_list);
Node *add(TokenList &token_list);
Node *mul(TokenList &token_list);
Node *unary(TokenList &token_list);
Node *primary(TokenList &token_list);

void program(TokenList &token_list)
{
    int i = 0;
    while (!token_list.at_end())
    {
        code[i] = statement(token_list);
        i++;
    }

    code[i] = nullptr;
}

Node *statement(TokenList &token_list)
{
    Node *node;
    if (token_list.consume_if(TokenKind::TK_RETURN))
    {
        node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
        node->kind = NodeKind::ND_RETURN;
        node->lhs = expr(token_list);
        token_list.expect(";");
    }
    else if (token_list.consume_if(TokenKind::TK_IF))
    {
        token_list.expect("(");
        Node *lhs = expr(token_list);
        token_list.expect(")");
        Node *rhs = statement(token_list);
        if (token_list.consume_if("else"))
        {
            node = new_node(NodeKind::ND_IFELSE, lhs, rhs, statement(token_list));
        }
        else
        {
            node = new_node(NodeKind::ND_IF, lhs, rhs);
        }
    }
    else if (token_list.consume_if(TokenKind::TK_WHILE))
    {
        token_list.expect("(");
        Node *lhs = expr(token_list);
        token_list.expect(")");
        node = new_node(NodeKind::ND_WHILE, lhs, statement(token_list));
    }
    else if (token_list.consume_if(TokenKind::TK_FOR))
    {
        token_list.expect("(");
        Node *initialize_term = expr(token_list);
        token_list.expect(";");
        Node *condition_term = expr(token_list);
        token_list.expect(";");
        Node *update_term = expr(token_list);
        token_list.expect(")");
        node = new_node(NodeKind::ND_FOR, initialize_term, condition_term, update_term, statement(token_list));
    }
    else
    {
        node = expr(token_list);
        token_list.expect(";");
    }

    return node;
}

Node *expr(TokenList &token_list)
{
    Node *node = assign(token_list);
    return node;
}

Node *assign(TokenList &token_list)
{
    Node *node = equality(token_list);
    if (token_list.consume("="))
    {
        node = new_node(NodeKind::ND_ASSIGN, node, assign(token_list));
    }

    return node;
}

Node *equality(TokenList &token_list)
{
    Node *node = relatinal(token_list);

    for (;;)
    {
        if (token_list.consume("=="))
        {
            node = new_node(NodeKind::ND_EQ, node, relatinal(token_list));
        }
        else if (token_list.consume("!="))
        {
            node = new_node(NodeKind::ND_NE, node, relatinal(token_list));
        }
        else
        {
            return node;
        }
    }
}

Node *relatinal(TokenList &token_list)
{
    Node *node = add(token_list);

    for (;;)
    {
        if (token_list.consume("<"))
        {
            node = new_node(NodeKind::ND_LT, node, add(token_list));
        }
        else if (token_list.consume("<="))
        {
            node = new_node(NodeKind::ND_LE, node, add(token_list));
        }
        if (token_list.consume(">"))
        {
            node = new_node(NodeKind::ND_LT, add(token_list), node);
        }
        else if (token_list.consume(">="))
        {
            node = new_node(NodeKind::ND_LE, add(token_list), node);
        }
        else
        {
            return node;
        }
    }
}

Node *add(TokenList &token_list)
{
    Node *node = mul(token_list);

    for (;;)
    {
        if (token_list.consume("+"))
        {
            node = new_node(NodeKind::ND_ADD, node, mul(token_list));
        }
        else if (token_list.consume("-"))
        {
            node = new_node(NodeKind::ND_SUB, node, mul(token_list));
        }
        else
        {
            return node;
        }
    }
}

Node *mul(TokenList &token_list)
{
    Node *node = unary(token_list);

    for (;;)
    {
        if (token_list.consume("*"))
        {
            node = new_node(NodeKind::ND_MUL, node, unary(token_list));
        }
        else if (token_list.consume("/"))
        {
            node = new_node(NodeKind::ND_DIV, node, unary(token_list));
        }
        else
        {
            return node;
        }
    }
}

Node *unary(TokenList &token_list)
{
    if (token_list.consume("+"))
    {
        return primary(token_list);
    }
    if (token_list.consume("-"))
    {
        return new_node(NodeKind::ND_SUB, new_node_num(0), primary(token_list));
    }
    return primary(token_list);
}

Node *primary(TokenList &token_list)
{
    if (token_list.consume("("))
    {
        Node *node = expr(token_list);
        token_list.expect(")");
        return node;
    }

    switch (token_list.getCurrentToken()->mKind)
    {
    case TokenKind::TK_IDENT:
    {
        const Token &lvar_token = *token_list.expect_lvar();
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
        return new_node_num(token_list.expect_number());
        break;

    default:
        error("ここに到達することは構文違反です。\n");
        exit(1);
    }
}
