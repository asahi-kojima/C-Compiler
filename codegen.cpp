
#include "9cc.h"

void gen_lval(Node& node)
{
    if (node.kind != NodeKind::ND_LVAR)
    {
        error("代入の左辺値が変数ではありません。");
    }

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node.offset);
    printf("    push rax\n");
}


void gen(Node &node)
{
    switch (node.kind)
    {
    case NodeKind::ND_NUM:
        printf("    push %d\n", node.val);
        return;

    case NodeKind::ND_LVAR:
        gen_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;

    case NodeKind::ND_ASSIGN:
        gen_lval(*node.lhs);
        gen(*node.rhs);
        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;
    }

    gen(*node.lhs);
    gen(*node.rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node.kind)
    {

    // +
    case NodeKind::ND_ADD:
        printf("    add rax, rdi\n");
        break;

    // -
    case NodeKind::ND_SUB:
        printf("    sub rax, rdi\n");
        break;

    // *
    case NodeKind::ND_MUL:
        printf("    imul rax, rdi\n");
        break;

    // /
    case NodeKind::ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;

    // ==
    case NodeKind::ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;

    // !=
    case NodeKind::ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;

    // <
    case NodeKind::ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;

    // <=
    case NodeKind::ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;

    // can't reach here
    default:
        fprintf(stderr, "node error");
        exit(1);
        break;
    }

    printf("    push rax\n");
}