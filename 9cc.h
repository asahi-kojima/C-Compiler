#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    TK_IDENT,
    TK_NUM,
    TK_EOF,
};

class Token
{
public:
    Token(char* user_input) : token(nullptr) , user_input(user_input){}
    void tokenize();

    bool consume(const char* op);
    void expect(const char* op);
    int expect_number();
    bool is_lvar(int& offset);
    bool at_end() const;

    void setStr(const char* p)
    {
        str = p;
    }

private:
    TokenKind mKind;
    Token *mNext;
    int val;
    const char *str;
    int len;

    Token *token;
    char *user_input;

    Token *new_token(TokenKind kind, Token *cur, char *str, int len);

    void error_at(const char *location, const char *fmt, ...);
};


//parse.cpp
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
};


struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
    int offset;
};


extern Node* code[];
void program(Token& token);
Node* expr(Token&);



//codegen.cpp
void gen(Node& node);
