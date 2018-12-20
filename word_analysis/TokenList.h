//
// Created by wmj on 18-11-26.
//

#ifndef COMPLIE_TOKENLIST_H
#define COMPLIE_TOKENLIST_H

#include <string>
#include <vector>
#include "NumericDFA.h"
#include <iostream>

#define TOKEN_ID 'i'
#define TOKEN_CHAR 'h'
#define TOKEN_STR 's'
#define TOKEN_KEY 'k'
#define TOKEN_BOUND 'p'
#define TOKEN_ASM 'a'

struct Token {
    Token(char c, size_t i);
    char first;
    size_t second;
    friend std::ostream& operator<<(std::ostream& out, const Token& s);
};

std::ostream& operator<<(std::ostream& out, const Token& s);

class TokenList {

    std::vector<std::string> id;  // i
    std::vector<char> character;  // h
    std::vector<std::string> str;  // s
    std::vector<Number> constants;  // c
    std::vector<std::string> key;  // k
    std::vector<std::string> bound;  // p

    std::vector<Token> list;

public:
    typedef std::vector<Token>::const_iterator const_iterator;
    typedef std::vector<Token>::iterator iterator;

    iterator begin();
    iterator end();

    void add_id(const std::string& a);
    void add_char(char a);
    void add_str(const std::string& a);
    void add_num(const Number& a);
    void add_key(const std::string& a);
    void add_bound(const std::string& a);

    void set_bound_list(const std::vector<std::string>& l);
    void set_key_list(const std::vector<std::string>& l);

    char get_token_char(const Token &t) const;
    Number get_token_num(const Token &t) const;
    std::string get_token(const Token& t) const;
    std::string get_token_str(Token t) const;
    std::string get_identification(size_t t);
    std::string get_bound(size_t t);
    std::string get_key(size_t t);
    Number get_number(size_t);

    size_t size() const;

    void print_token(long s, long e) const;

    std::string get_grammar_token(Token&&);

};

class TokenGetter {
    TokenList& tkl;
    TokenList::iterator it;
    TokenList::iterator end;
public:
    explicit TokenGetter(TokenList& tkl);
    Token next();
    Token get();
};


#endif //COMPLIE_TOKENLIST_H
