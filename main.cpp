#include "9cc.h"




int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引き数の個数が正しくありません。");
        return 1;
    }

    //トークンに分割する
    char *user_input = argv[1];
    Token token = Token(user_input);
    token.tokenize();
    program(token);


    //アセンブリの定型文
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    //ローカル変数のためのスタックメモリを確保する。
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, 208\n");
    //抽象構文木を生成する
    for (int i = 0; code[i]; i++)
    {
        gen(*code[i]);
        printf("    pop rax\n");
    }

    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}