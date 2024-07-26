
#include "9cc.h"

void gen_lval(Node &node)
{
    if (node.kind != NodeKind::ND_LVAR)
    {
        error("代入の左辺値が変数ではありません。");
    }

    printf("    mov rax, rbp\n");
    printf("    sub rax, %d\n", node.offset);
    printf("    push rax\n");
}


void gen(Node &node)
{
    static int label_identifier = 0;
    switch (node.kind)
    {
    case NodeKind::ND_NUM:
        printf("    push %d\n", node.val);
        return;

    case NodeKind::ND_LVAR:
        gen_lval(node);
        printf("    pop rax\n");
        printf("    mov rax, [rax]\n");
        printf("    push rax\n");
        return;

    case NodeKind::ND_ASSIGN:
        gen_lval(*node.lhs);
        gen(*node.rhs);
        printf("    pop rdi\n");
        printf("    pop rax\n");
        printf("    mov [rax], rdi\n");
        printf("    push rdi\n");
        return;

    case NodeKind::ND_RETURN:
        gen(*node.lhs);
        printf("    pop rax\n");
        printf("    mov rsp, rbp\n");
        printf("    pop rbp\n");
        printf("    ret\n");
        return;

    case NodeKind::ND_IFELSE:
        gen(*node.lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lelse%d\n", label_identifier);
        gen(*node.rhs);
        printf("    jmp .Lend%d\n", label_identifier);
        printf(".Lelse%d:\n", label_identifier);
        gen(*node.ths);
        printf(".Lend%d:\n", label_identifier);
        label_identifier++;

        return;
    
    case NodeKind::ND_IF:
        gen(*node.lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", label_identifier);
        gen(*node.rhs);
        printf(".Lend%d:\n", label_identifier);
        label_identifier++;

        return;

    case NodeKind::ND_WHILE:
        printf(".Lbegin%d:\n", label_identifier);
        gen(*node.lhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", label_identifier);
        gen(*node.rhs);
        printf("    jmp .Lbegin%d\n", label_identifier);
        printf(".Lend%d:\n", label_identifier);
        label_identifier++;

        return;

    case NodeKind::ND_FOR:
        gen(*node.lhs);
        printf(".Lbegin%d:\n", label_identifier);
        gen(*node.rhs);
        printf("    pop rax\n");
        printf("    cmp rax, 0\n");
        printf("    je .Lend%d\n", label_identifier);
        gen(*node.fhs);
        gen(*node.ths);
        printf("    jmp .Lbegin%d\n", label_identifier);
        printf(".Lend%d:\n", label_identifier);
        label_identifier++;

        return;

    case NodeKind::ND_BLOCK:
        printf("    push 1\n");
        for (const auto& child : node.node_in_block)
        {
            printf("    pop rax\n");
            gen(*child);
        }

        return;

    //関数コール
    case NodeKind::ND_FUNCCALL:
        const char* register_name_tbl[6] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
        for (int i = 0;const auto& child : node.args)
        {
            if (i >= 6)
            {
                fprintf(stderr, "関数の引数が多すぎます。現在の実装では６個までです。\n");
                exit(1);
            }
            gen(*child);
            printf("    pop %s\n", register_name_tbl[i]);

            i++;
        }

        //RSPを16バイトアライメントに揃える
        //まずアライメントが維持されているかチェック
        printf("    mov rax, rsp\n");
        printf("    and rax, 0xf\n");
        printf("    test rax, rax\n");
        //されていれば以下はスキップ
        printf("    jz .Laligned\n");
        //補正して関数呼び出し
        printf("    sub rsp, 8\n");
        printf("    call %s\n", node.str);
        //補正した分を戻す
        printf("    add rsp, 8\n");
        printf("    jmp .Lnotaligned\n");
        printf(".Laligned:\n");
        printf("    call %s\n", node.str);
        printf(".Lnotaligned:\n");
        printf("    push rax\n");
        return;
    }

    gen(*node.lhs);
    gen(*node.rhs);

    printf("    pop rdi\n");
    printf("    pop rax\n");

    switch (node.kind)
    {

    // +
    case NodeKind::ND_ADD:
        printf("    add rax, rdi\n");
        break;

    // -
    case NodeKind::ND_SUB:
        printf("    sub rax, rdi\n");
        break;

    // *
    case NodeKind::ND_MUL:
        printf("    imul rax, rdi\n");
        break;

    // /
    case NodeKind::ND_DIV:
        printf("    cqo\n");
        printf("    idiv rdi\n");
        break;

    // ==
    case NodeKind::ND_EQ:
        printf("    cmp rax, rdi\n");
        printf("    sete al\n");
        printf("    movzb rax, al\n");
        break;

    // !=
    case NodeKind::ND_NE:
        printf("    cmp rax, rdi\n");
        printf("    setne al\n");
        printf("    movzb rax, al\n");
        break;

    // <
    case NodeKind::ND_LT:
        printf("    cmp rax, rdi\n");
        printf("    setl al\n");
        printf("    movzb rax, al\n");
        break;

    // <=
    case NodeKind::ND_LE:
        printf("    cmp rax, rdi\n");
        printf("    setle al\n");
        printf("    movzb rax, al\n");
        break;

    // can't reach here
    default:
        fprintf(stderr, "node error");
        exit(1);
        break;
    }

    printf("    push rax\n");
}