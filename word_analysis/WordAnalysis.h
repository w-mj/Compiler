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

class WordAnalysis {
private:
    std::vector<std::string> id;
    std::vector<char> character;
    std::vector<std::string> str;
    std::vector<Number> constants;
    std::vector<std::string> key;
    std::vector<std::string> bound;

    PrefixTree key_tree;
    PrefixTree id_tree;
    PrefixTree bound_tree;

    std::vector<std::pair<char, int>> token;

    int getEscape(std::string::iterator s, std::string::iterator e);
public:
    WordAnalysis();
    bool process_key(std::string::iterator& iter, const std::string::iterator& end);
    bool process_id(std::string::iterator& iter, const std::string::iterator& end);
    bool process_str(std::string::iterator& iter, const std::string::iterator& end);
    bool process_char(std::string::iterator& iter, const std::string::iterator& end);
    bool process_constant(std::string::iterator& iter, const std::string::iterator &end);
    bool process_bound(std::string::iterator& iter, const std::string::iterator &end);
    const std::vector<std::pair<char, int>>& getToken();

    std::string token2str(long i);
};


#endif //COMPLIE_WORDANALYSIS_H