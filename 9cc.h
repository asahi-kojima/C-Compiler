#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <iostream>

#define LOCAL_VAR_SIZE 8

inline void error(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

enum class TokenKind
{
    TK_RESERVED,
    TK_IF,
    TK_WHILE,
    TK_FOR,
    TK_RETURN,
    TK_IDENT,
    TK_NUM,
    TK_EOF,
};

class Token
{
public:
    TokenKind mKind;
    Token *mNext;
    int val;
    const char *str;
    int len;
};

class TokenList
{
public:
    TokenList(char *user_input);
    void tokenize();

    bool consume(const char *op);
    bool consume_if(const char*);
    bool consume_if(TokenKind);
    void expect(const char *op);
    int expect_number();
    Token *expect_lvar();
    bool at_end() const;
    void error_at(const char *location, const char *fmt, ...);

    const Token *getCurrentToken() const
    {
        return current_token;
    }

private:
    Token *current_token;
    char *user_input;

    Token *new_token(TokenKind kind, Token *cur, const char *str, int len);
};

// parse.cpp
enum class NodeKind
{
    ND_ADD,
    ND_SUB,
    ND_MUL,
    ND_DIV,

    ND_EQ,
    ND_NE,
    ND_LT,
    ND_LE,

    ND_NUM,
    ND_ASSIGN,
    ND_LVAR,

    ND_RETURN,

    ND_IF,
    ND_IFELSE,
    ND_WHILE,
    ND_FOR,

    ND_BLOCK,

    ND_FUNCCALL,
};

struct LVar
{
    LVar *next;
    const char *name;
    int len;
    int offset;
};

struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    Node *ths;//third hs
    Node *fhs;//fourth hs
    int val;
    int offset;

    const char* str;

    std::vector<Node*> node_in_block;
    std::vector<Node*> args;
};

extern LVar *locals;
extern Node *code[];
void program(TokenList &token_list);

// codegen.cpp
void gen(Node &node);
