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
    printf("    sub rax, %d\n", node->property.of_ident.offset * LOCAL_VARIABLE_BYTE_SIZE);
    printf("    push rax\n");
}


//構文木の左右をコンパイルし、特定の計算を行い、計算結果をスタックトップに置く。
void GenerateAssemblyCode(AstNode* node)
{
    static u32 label_id = 0;
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

    case AstNodeKind::ND_RETURN:
        GenerateAssemblyCode(node->lhs_node);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;

    case AstNodeKind::ND_IF:
        {
            GenerateAssemblyCode(node->condition_node);
            printf("    pop rax\n"); //条件式の結果をraxに格納
            printf("    cmp rax, 0\n");//raxの値(コンディションがtrueなら1、falseなら0)が0と等しい場合にalレジスタに1をセット
            printf("    je .Lelse%d\n", label_id);
            GenerateAssemblyCode(node->lhs_node);
            printf("    jmp .Lend%d\n", label_id);
            printf(".Lelse%d:\n", label_id);
            if (node->rhs_node != nullptr)
            {
                GenerateAssemblyCode(node->rhs_node);
            }
            printf(".Lend%d:\n", label_id); //条件が真ならば、スタックにraxを積む
            
            label_id++;
        }
        return;

    case AstNodeKind::ND_WHILE:
        {
            printf(".Lbegin%d:\n", label_id);
            GenerateAssemblyCode(node->condition_node);
            printf("    pop rax\n");
            printf("    cmp rax, 0\n");
            printf("    je .Lend%d\n", label_id);
            GenerateAssemblyCode(node->lhs_node);
            printf("    jmp .Lbegin%d\n", label_id);
            printf("    pop rax\n");
            printf(".Lend%d:\n", label_id);
            printf("    push 0\n");//計算終了後にスタックに積まれた値を除去する箇所があるが、そこと相殺するようにダミーを置いておく。
            
            label_id++;
        }
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