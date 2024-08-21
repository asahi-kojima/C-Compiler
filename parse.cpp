#include "9cc.h"

static std::string current_function_name;

std::map<std::string, int> function_argumentnum_table;
std::map<std::string, LVar *> function_lvar_table;
std::map<std::string, std::vector<Node *>> function_code_table;

namespace
{

    Node *new_node(NodeKind kind, Node *lhs, Node *rhs, Node *ths = nullptr, Node *fhs = nullptr)
    {
        Node *node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
        node->kind = kind;
        node->lhs = lhs;
        node->rhs = rhs;
        node->ths = ths;
        node->fhs = fhs;
        return node;
    }

    Node *new_node_num(int num)
    {
        Node *node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
        node->kind = NodeKind::ND_NUM;
        node->val = num;
        return node;
    }

    Node *new_node_lvar(int offset)
    {
        Node *node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
        node->kind = NodeKind::ND_LVAR;
        node->offset = offset;
        return node;
    }

    LVar *find_lvar(const Token &token)
    {
        for (LVar *var = function_lvar_table[current_function_name]; var; var = var->next)
        {
            if (var->len == token.len && !memcmp(token.str, var->name, var->len))
            {
                return var;
            }
        }

        return nullptr;
    }

    void make_new_lvar(const Token &token)
    {
        LVar *new_lvar = reinterpret_cast<LVar *>(calloc(1, sizeof(LVar)));
        new_lvar->len = token.len;
        new_lvar->name = token.str;

        LVar *lvar = function_lvar_table[current_function_name];
        if (lvar)
        {
            for (; lvar->next; lvar = lvar->next)
            {
            }
            new_lvar->offset = lvar->offset + LOCAL_VAR_SIZE;
            lvar->next = new_lvar;
        }
        else
        {
            new_lvar->offset = 1 * LOCAL_VAR_SIZE;
            function_lvar_table[current_function_name] = new_lvar;
        }
    }
}

void program(TokenList &token_list);
Node *statement(TokenList &token_list);
Node *expr(TokenList &token_list);
Node *assign(TokenList &token_list);
Node *equality(TokenList &token_list);
Node *relatinal(TokenList &token_list);
Node *add(TokenList &token_list);
Node *mul(TokenList &token_list);
Node *unary(TokenList &token_list);
Node *primary(TokenList &token_list);

void program(TokenList &token_list)
{
    // 関数名(引数){関数定義}の順になっているはず
    while (!token_list.at_end())
    {
        // 関数名が来るはず
        const Token &current = *token_list.getCurrentToken();
        if (token_list.consume_if(TokenKind::TK_IDENT))
        {
            // 現在解析している関数名を静的変数に格納して後続の処理でも参照できるようにする
            current_function_name = std::string(current.str, current.len);

            // 引数が来るはず
            token_list.consume("(");
            int argument_no = 0;
            for (; !token_list.consume_if(")"); argument_no++)
            {
                if (argument_no >= 6)
                {
                    fprintf(stderr, "arguments must be less than 6.\n");
                    exit(1);
                }

                if (argument_no != 0)
                {
                    token_list.expect(",");
                }

                const auto &lvar_token = *token_list.expect_lvar();
                LVar *lvar = find_lvar(lvar_token);
                if (lvar)
                {
                    fprintf(stderr, "argument symbol overlapping.\n");
                    exit(1);
                }
                else
                {
                    make_new_lvar(lvar_token);
                }
            }
            function_argumentnum_table[current_function_name] = argument_no;

            function_lvar_table[current_function_name] = nullptr;

            token_list.consume("{");
            // 関数定義
            std::vector<Node *> codes;
            while (!token_list.consume_if("}"))
            {
                codes.push_back(statement(token_list));
            }
            codes.push_back(nullptr);

            function_code_table[current_function_name] = std::move(codes);
        }
        else
        {
            printf("function name must come.\n");
            exit(1);
        }
    }
}

Node *statement(TokenList &token_list)
{
    Node *node;
    if (token_list.consume_if(TokenKind::TK_RETURN))
    {
        node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
        node->kind = NodeKind::ND_RETURN;
        node->lhs = expr(token_list);
        token_list.expect(";");
    }
    else if (token_list.consume_if(TokenKind::TK_IF))
    {
        token_list.expect("(");
        Node *lhs = expr(token_list);
        token_list.expect(")");
        Node *rhs = statement(token_list);
        if (token_list.consume_if("else"))
        {
            node = new_node(NodeKind::ND_IFELSE, lhs, rhs, statement(token_list));
        }
        else
        {
            node = new_node(NodeKind::ND_IF, lhs, rhs);
        }
    }
    else if (token_list.consume_if(TokenKind::TK_WHILE))
    {
        token_list.expect("(");
        Node *lhs = expr(token_list);
        token_list.expect(")");
        node = new_node(NodeKind::ND_WHILE, lhs, statement(token_list));
    }
    else if (token_list.consume_if(TokenKind::TK_FOR))
    {
        token_list.expect("(");
        Node *initialize_term = expr(token_list);
        token_list.expect(";");
        Node *condition_term = expr(token_list);
        token_list.expect(";");
        Node *update_term = expr(token_list);
        token_list.expect(")");
        node = new_node(NodeKind::ND_FOR, initialize_term, condition_term, update_term, statement(token_list));
    }
    else if (token_list.consume("{"))
    {
        node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
        node->kind = NodeKind::ND_BLOCK;
        node->node_in_block.clear();
        do
        {
            Node *node_in_block = statement(token_list);
            node->node_in_block.push_back(node_in_block);
        } while (!token_list.consume("}"));
    }
    else
    {
        node = expr(token_list);
        token_list.expect(";");
    }

    return node;
}

Node *expr(TokenList &token_list)
{
    Node *node = assign(token_list);
    return node;
}

Node *assign(TokenList &token_list)
{
    Node *node = equality(token_list);
    if (token_list.consume("="))
    {
        node = new_node(NodeKind::ND_ASSIGN, node, assign(token_list));
    }

    return node;
}

Node *equality(TokenList &token_list)
{
    Node *node = relatinal(token_list);

    for (;;)
    {
        if (token_list.consume("=="))
        {
            node = new_node(NodeKind::ND_EQ, node, relatinal(token_list));
        }
        else if (token_list.consume("!="))
        {
            node = new_node(NodeKind::ND_NE, node, relatinal(token_list));
        }
        else
        {
            return node;
        }
    }
}

Node *relatinal(TokenList &token_list)
{
    Node *node = add(token_list);

    for (;;)
    {
        if (token_list.consume("<"))
        {
            node = new_node(NodeKind::ND_LT, node, add(token_list));
        }
        else if (token_list.consume("<="))
        {
            node = new_node(NodeKind::ND_LE, node, add(token_list));
        }
        if (token_list.consume(">"))
        {
            node = new_node(NodeKind::ND_LT, add(token_list), node);
        }
        else if (token_list.consume(">="))
        {
            node = new_node(NodeKind::ND_LE, add(token_list), node);
        }
        else
        {
            return node;
        }
    }
}

Node *add(TokenList &token_list)
{
    Node *node = mul(token_list);

    for (;;)
    {
        if (token_list.consume("+"))
        {
            node = new_node(NodeKind::ND_ADD, node, mul(token_list));
        }
        else if (token_list.consume("-"))
        {
            node = new_node(NodeKind::ND_SUB, node, mul(token_list));
        }
        else
        {
            return node;
        }
    }
}

Node *mul(TokenList &token_list)
{
    Node *node = unary(token_list);

    for (;;)
    {
        if (token_list.consume("*"))
        {
            node = new_node(NodeKind::ND_MUL, node, unary(token_list));
        }
        else if (token_list.consume("/"))
        {
            node = new_node(NodeKind::ND_DIV, node, unary(token_list));
        }
        else
        {
            return node;
        }
    }
}

Node *unary(TokenList &token_list)
{
    if (token_list.consume("+"))
    {
        return primary(token_list);
    }
    if (token_list.consume("-"))
    {
        return new_node(NodeKind::ND_SUB, new_node_num(0), primary(token_list));
    }
    return primary(token_list);
}

Node *primary(TokenList &token_list)
{
    if (token_list.consume("("))
    {
        Node *node = expr(token_list);
        token_list.expect(")");
        return node;
    }

    switch (token_list.getCurrentToken()->mKind)
    {
    case TokenKind::TK_IDENT:
    {
        const Token &lvar_token = *token_list.expect_lvar();

        if (token_list.consume("("))
        {
            Node *node = reinterpret_cast<Node *>(calloc(1, sizeof(Node)));
            node->kind = NodeKind::ND_FUNCCALL;
            char *str = new char[lvar_token.len + 1];
            str[lvar_token.len] = '\0';
            memcpy(str, lvar_token.str, lvar_token.len);
            node->str = str;

            for (int i = 0; !token_list.consume(")"); i++)
            {
                if (i != 0)
                {
                    token_list.expect(",");
                }
                Node *arg = expr(token_list);
                node->args.push_back(arg);
            }

            return node;
        }

        LVar *lvar = find_lvar(lvar_token);
        if (lvar)
        {
            return new_node_lvar(lvar->offset);
        }
        else
        {
            make_new_lvar(lvar_token);
            LVar *lvar = find_lvar(lvar_token);
            return new_node_lvar(lvar->offset);
        }
        break;
    }

    case TokenKind::TK_NUM:
        return new_node_num(token_list.expect_number());
        break;

    default:
        error("ここに到達することは構文違反です。現在のトークンは%cです。\n", *token_list.getCurrentToken()->str);
        exit(1);
    }
}
