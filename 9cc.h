#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    bool at_end() const;

private:
    TokenKind mKind;
    Token *mNext;
    int val;
    char *str;
    int len;

    Token *token;
    char *user_input;

    Token *new_token(TokenKind kind, Token *cur, char *str, int len);

    void error(const char *fmt, ...);

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
};


struct Node
{
    NodeKind kind;
    Node *lhs;
    Node *rhs;
    int val;
};


Node* expr(Token& token);




//codegen.cpp
void gen(Node& node);
