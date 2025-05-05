#pragma once
#include "essential.h"
#include <vector>

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


struct Token
{
    //トークンタイプ
    TokenKind token_kind;

    //このトークンの入力コード内での場所
    //入力コード列をp,先頭からNbyte目からトークン文字列が始まっていればp+Nが入る
    const char* token_position_in_input_string;

    //トークンタイプ毎にプロパティが必要になるので、それを格納しておくユニオン型変数
    union TokenProperty
    {
        struct OfNum {s32 value;} of_num;
        struct OfString {const char* str;  s32 len;} of_string;
    } property;

    Token(TokenKind kind, const char* token_position_in_input_string, TokenProperty property)
    : token_kind(kind)
    , token_position_in_input_string(token_position_in_input_string)
    , property(property){}
};

class TokenStream
{
public:
    TokenStream(char* input_string);

    //現在のトークンタイプがRESERVEDであり、かつ指定した文字列の場合にトークンを読み進める。
    bool consume_if(const char* str);

    //トークンがRESERVED型であり、指定した文字列である事を確認する。
    void expect(const char* str);

    //現在のトークンが数字である事を確認する。
    s32 expect_number();

    Token get_current_token() const;

    //トークン列が終了か判定する。
    bool at_end() const;

    //デバッグ関数
    void print_all_tokens() const;


private:
    //トークン分割を行う関数
    void tokenize();

    //分割したトークンを格納するリスト
    std::vector<Token> m_token_list;

    //トークン列を指すイテレーターと終端イテレーター
    std::vector<Token>::iterator m_current_token_iter;
    std::vector<Token>::iterator m_end_token_iter;

    //入力されたコードを保存しておく変数：エラー表示に利用
    char* m_input_string = nullptr;
};