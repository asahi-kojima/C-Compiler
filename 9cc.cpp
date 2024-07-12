#include <stdio.h>
#include <stdlib.h>
#include "Tokenizer.h"






int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引き数の個数が正しくありません。");
        return 1;
    }
    char* user_input = argv[1];
    Token token = Token(user_input);
    token.tokenize();

    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    printf("    mov rax, %ld\n", token.expect_number());

    while (!token.at_end())
    {
        if (token.consume('+'))
        {
            printf("    add rax, %ld\n", token.expect_number());
            continue;
        }

        token.expect('-');
        printf("    sub rax, %ld\n", token.expect_number());
    }

    printf("    ret\n");
    return 0;
}