#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"

//ここは慣習に従って通常の型を利用
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "argument num must be 2! but you set %d args", argc);
    }

    TokenStream token_stream(argv[1]);

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");
    printf("    mov rax, %d\n", token_stream.expect_number());

    while (!token_stream.at_end())
    {
        if (token_stream.consume_if("+"))
        {
            printf("    add rax, %d\n", token_stream.expect_number());
            continue;
        }

        token_stream.expect("-");
        printf("    sub rax, %d\n", token_stream.expect_number());
    }

    printf("    ret\n");

    return 0;
}