#include "9cc.h"

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "引き数の個数が正しくありません。");
        return 1;
    }

    // トークンに分割する
    char *user_input = argv[1];
    Token token = Token(user_input);
    token.tokenize();

    //抽象構文木の構築
    program(token);

    //ローカル変数用のスタックのサイズを計算している。
    int local_var_stack_size = 0;
    if (locals)
    {
        LVar *lvar = locals;
        for (;lvar->next; lvar = lvar->next)
        {
        }
        local_var_stack_size = lvar->offset;
    }
    
    // アセンブリの定型文:call編
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");
    printf("main:\n");

    // ローカル変数のためのスタックメモリを確保する。
    printf("    push rbp\n");
    printf("    mov rbp, rsp\n");
    printf("    sub rsp, %d\n", local_var_stack_size);
    // 抽象構文木からアセンブリコードを生成
    for (int i = 0; code[i]; i++)
    {
        gen(*code[i]);
        printf("    pop rax\n");
    }
    //アセンブリの定型文:ret編
    printf("    mov rsp, rbp\n");
    printf("    pop rbp\n");
    printf("    ret\n");
    return 0;
}