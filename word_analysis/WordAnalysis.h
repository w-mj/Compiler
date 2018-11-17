//
// Created by wmj on 18-11-1.
//

#ifndef COMPLIE_WORDANALYSIS_H
#define COMPLIE_WORDANALYSIS_H

#include <vector>
#include <string>
#include <map>
#include "PrefixTree.h"
#include "NumericDFA.h"
typedef std::pair<char, int> Token;
typedef std::vector<Token> TokenList;
class WordAnalysis {
private:
    std::vector<std::string> id;  // i
    std::vector<char> character;  // h
    std::vector<std::string> str;  // s
    std::vector<Number> constants;  // c
    std::vector<std::string> key;  // k
    std::vector<std::string> bound;  // p

    PrefixTree key_tree;
    PrefixTree id_tree;
    PrefixTree bound_tree;

    TokenList token;

    int getEscape(std::string::iterator s, std::string::iterator e);
public:
    WordAnalysis();
    bool process_key(std::string::iterator& iter, const std::string::iterator& end);
    bool process_id(std::string::iterator& iter, const std::string::iterator& end);
    bool process_str(std::string::iterator& iter, const std::string::iterator& end);
    bool process_char(std::string::iterator& iter, const std::string::iterator& end);
    bool process_constant(std::string::iterator& iter, const std::string::iterator &end);
    bool process_bound(std::string::iterator& iter, const std::string::iterator &end);
    const std::vector<std::pair<char, int>>& get_token_list() const;

    void process_file(std::ifstream &file, bool print=false);

    std::string get_token(const Token& t) const;
    char get_token_char(const Token& t) const;
    Number get_token_num(const Token& t) const;


    std::string token2str(long i);
};


#endif //COMPLIE_WORDANALYSIS_H
