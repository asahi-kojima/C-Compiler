#include <stdio.h>
#include <stdlib.h>
#include "code-generator.h"


void push_address_of_lvar(AstNode* node)
{
    if (node->kind != AstNodeKind::ND_LVAR)
    {
        fprintf(stderr, "代入の左辺に右辺値が来ています。\n");
        exit(1);
    }

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", (node->property.of_ident.name[0] - 'a') * 8);
    printf("    push rax\n");
}


//構文木の左右をコンパイルし、特定の計算を行い、計算結果をスタックトップに置く。
void GenerateAssemblyCode(AstNode* node)
{
    switch (node->kind)
    {
    case AstNodeKind::ND_NUM:
        printf("    push %d\n", node->property.of_num.value);
        return;
    
    case AstNodeKind::ND_LVAR:
        push_address_of_lvar(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;
    
    case AstNodeKind::ND_ASSIGN:
        push_address_of_lvar(node->lhs_node);
        GenerateAssemblyCode(node->rhs_node);

        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;

    default:
        //終端記号以外はここを通過し、さらにコンパイル作業が続く。
        ;
    }

    GenerateAssemblyCode(node->lhs_node);
    GenerateAssemblyCode(node->rhs_node);

    //スタックからのポップなので、後半のコンパイル結果が先にrdiに格納される。
    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node->kind)
    {
    case AstNodeKind::ND_ADD:
        printf("    add rax, rdi\n");
        break;

    case AstNodeKind::ND_SUB:
        printf("    sub rax, rdi\n");
        break; 
    
    case AstNodeKind::ND_MUL:
        printf("    imul rax, rdi\n");
        break;

    case AstNodeKind::ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;

    case AstNodeKind::ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;

    case AstNodeKind::ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;

    case AstNodeKind::ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;

    case AstNodeKind::ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;

    default:
        fprintf(stderr, "can't reach here! : error in code-generator\n");
        exit(1);
    }

    printf("    push rax\n");
}