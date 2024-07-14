#include <stdio.h>
#include <stdlib.h>
#include "Tokenizer.h"

enum class NodeKind
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

struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};

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


Node *equality(Token &token);
Node *relatinal(Token &token);
Node *add(Token &token);
Node *mul(Token &token);
Node *unary(Token &);
Node *primary(Token &);

Node *expr(Token &token)
{
    Node *node = equality(token);

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

    return new_node_num(token.expect_number());
}

void gen(Node &node)
{
    if (node.kind == NodeKind::ND_NUM)
    {
        printf("    push %d\n", node.val);
        return;
    }

    gen(*node.lhs);
    gen(*node.rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node.kind)
    {
    case NodeKind::ND_ADD:
        printf("    add rax, rdi\n");
        break;
    case NodeKind::ND_SUB:
        printf("    sub rax, rdi\n");
        break;

    case NodeKind::ND_MUL:
        printf("    imul rax, rdi\n");
        break;

    case NodeKind::ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;

    case NodeKind::ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;

    case NodeKind::ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;

    case NodeKind::ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;

    case NodeKind::ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;

    default:
        fprintf(stderr, "node error");
        exit(1);
        break;
    }

    printf("    push rax\n");
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引き数の個数が正しくありません。");
        return 1;
    }
    char *user_input = argv[1];
    Token token = Token(user_input);
    token.tokenize();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    Node *node = expr(token);
    gen(*node);

    printf("    pop rax\n");
    printf("    ret\n");
    return 0;
}