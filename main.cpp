#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "parser.h"
#include "code-generator.h"

//ここは慣習に従って通常の型を利用
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "argument num must be 2! but you set %d args", argc);
    }

    TokenStream token_stream(argv[1]);
    
    Parser parser(&token_stream);
    AstNode* node = parser.expr();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    GenerateAssemblyCode(node);

    printf("    pop rax\n");
    printf("    ret\n");

    return 0;
}