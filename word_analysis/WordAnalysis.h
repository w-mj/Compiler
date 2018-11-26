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
#include "TokenList.h"

class WordAnalysis {
private:

    std::vector<std::string> key;
    std::vector<std::string> bound;

    PrefixTree key_tree;
    PrefixTree id_tree;
    PrefixTree bound_tree;

    TokenList tokenList;

    int getEscape(std::string::iterator s, std::string::iterator e);
public:
    explicit WordAnalysis();
    bool process_key(std::string::iterator& iter, const std::string::iterator& end);
    bool process_id(std::string::iterator& iter, const std::string::iterator& end);
    bool process_str(std::string::iterator& iter, const std::string::iterator& end);
    bool process_char(std::string::iterator& iter, const std::string::iterator& end);
    bool process_constant(std::string::iterator& iter, const std::string::iterator &end);
    bool process_bound(std::string::iterator& iter, const std::string::iterator &end);

    void process_file(std::ifstream &file, bool print=false);
    TokenList get_tokenList() const;

};

std::ostream& print_token(std::ostream& out, const TokenList& tk, long s, long e);


#endif //COMPLIE_WORDANALYSIS_H
