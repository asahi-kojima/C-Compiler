#include <stdio.h>
#include <stdlib.h>
#include "tokenizer.h"
#include "parser.h"
#include "code-generator.h"

//ここは慣習に従って通常の型を利用
int main(int argc, char** argv)
{
    //C言語のコードを入力してもらう必要があり、かつそれ以外は要らないのでエラーを出す。
    if (argc != 2)
    {
        fprintf(stderr, "argument num must be 2! but you set %d args", argc);
        exit(1);
    }

    //入力された文字列をトークン化する。
    //入力文字列はargv[1]に格納されている。
    TokenStream token_stream(argv[1]);
#ifdef DEBUG
    token_stream.print_all_tokens();
#endif

    //トークン化された文字列をパースして、抽象構文木を生成する。
    Parser parser(&token_stream);
    std::vector<FunctionRecord> program = parser.program();

#ifdef DEBUG
    // --- [DEBUG] ---
    // 生成された抽象構文木を標準エラー出力に表示
    fprintf(stderr, "--- Abstract Syntax Tree ---\n");
    for (const auto& function_record : program)
    {
        const std::string& function_name = function_record.get_name();
        const auto& nodes = function_record.get_nodes();
        
        fprintf(stderr, "Function: %s\n", function_name.c_str());
        for (const auto& node : nodes)
        {
            print_ast(node);
        }
    }
    fprintf(stderr, "--------------------------\n\n");
#endif

    //アセンブリコードの定型文
    printf(".intel_syntax noprefix\n");
    printf(".global main\n");

    //関数ごとにアセンブリコードを生成する。
    for (const auto& function_record : program)
    {
        const std::string& function_name = function_record.get_name();
        const auto& nodes = function_record.get_nodes();
        const u32 stack_size = function_record.get_stack_size();

        printf("%s:\n", function_name.c_str());
        {
            //プロローグ
            printf("    push rbp\n");
            printf("    mov rbp, rsp\n");
            printf("    sub rsp, %d\n", stack_size);

            for (auto iter = nodes.begin(), end = nodes.end(); iter != end; iter++)
            {
                GenerateAssemblyCode(*iter);

                //コンパイル結果がスタックに積まれていってしまうので、raxに適当に退避させる。
                //必要でないなら上書きしても問題ない
                printf("    pop rax\n");
            }

            //関数の返り値はraxに格納されるが、上記の処理でスタック最上部に乗っている値をraxに取り出しているので、
            //このままリターンして問題ない。

            //エピローグ
            printf("    mov rsp, rbp\n");
            printf("    pop rbp\n");
            printf("    ret\n");
        }
    }

    return 0;
}