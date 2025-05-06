#include <stdio.h>
#include <stdlib.h>
#include "code-generator.h"



//構文木の左右をコンパイルし、特定の計算を行い、スタックトップに
void GenerateAssemblyCode(AstNode* node)
{
    if (node->kind == AstNodeKind::ND_NUM)
    {
        printf("    push %d\n", node->property.of_num.value);
        return;
    }

    //終端記号の場合はここまでで終わるはず
    //一方で比終端記号の場合はここから更にコンパイル作業が続く

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

    default:
        fprintf(stderr, "can't reach here!\n");
        exit(1);
    }

    printf("    push rax\n");
}