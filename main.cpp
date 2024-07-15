#include "9cc.h"




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