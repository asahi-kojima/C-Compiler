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
    TokenList token_list = TokenList(user_input);

    // 抽象構文木の構築
    program(token_list);

    // アセンブリの定型文:call編
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");

    for (auto iter = function_code_table.begin(), end = function_code_table.end(); iter != end; iter++)
    {
        const auto &function_name = iter->first; // 関数名
        const auto &codetree = iter->second;     // 関数定義の抽象構文木

        int argument_num = function_argumentnum_table[function_name];
        LVar *lvar_table = function_lvar_table[function_name];

        // ローカル変数用のスタックのサイズを計算している。
        int local_var_stack_size = 0;
        if (lvar_table)
        {
            LVar *lvar = lvar_table;
            for (; lvar->next; lvar = lvar->next)
            {
            }
            local_var_stack_size = lvar->offset;
        }

        
        printf("%s:\n", function_name.c_str());
        // アセンブリの定型文:プロローグ
        printf("    push rbp\n");
        printf("    mov rbp, rsp\n");
        printf("    sub rsp, %d\n", local_var_stack_size);
        for (int argument_no = 0; argument_no < argument_num; argument_no++)
        {
            printf("    mov [rsp + %d], %s\n",argument_no * LOCAL_VAR_SIZE,  register_name_tbl[argument_no]);
        }
        // 実際のコード部分
        for (int i = 0; codetree[i]; i++)
        {
            gen(*codetree[i]);
            printf("    pop rax\n");
        }
        // アセンブリの定型文:エピローグ
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
    }

    // printf("main:\n");

    // // ローカル変数のためのスタックメモリを確保する。
    // printf("    push rbp\n");
    // printf("    mov rbp, rsp\n");
    // printf("    sub rsp, %d\n", local_var_stack_size);
    // // 抽象構文木からアセンブリコードを生成
    // for (int i = 0; code[i]; i++)
    // {
    //     gen(*code[i]);
    //     printf("    pop rax\n");
    // }
    // // アセンブリの定型文:ret編
    // printf("    mov rsp, rbp\n");
    // printf("    pop rbp\n");
    // printf("    ret\n");
    return 0;
}