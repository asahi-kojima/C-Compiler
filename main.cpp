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
    std::vector<AstNode*> nodes = parser.program();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");

    //main関数のブロック
    printf("main:\n");
    {
        //プロローグ
        printf("    push rbp\n");
        printf("    mov rbp, rsp\n");
        printf("    sub rsp, 208\n");
        
        for (auto iter = nodes.begin(), end = nodes.end(); iter != end; iter++)
        {
            GenerateAssemblyCode(*iter);
            printf("    pop rax\n");
        }

        //エピローグ
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
    }
    return 0;
}