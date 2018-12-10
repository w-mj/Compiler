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

#define WA WordAnalysis::getInstance()
#define SL WA.get_tokenList()

class WordAnalysis {
private:



    PrefixTree key_tree;
    PrefixTree id_tree;
    PrefixTree bound_tree;

    TokenList tokenList;

    static std::map<std::string, int> key_map, bound_map;

    int getEscape(std::string::iterator s, std::string::iterator e);
    explicit WordAnalysis();

public:
    WordAnalysis(WordAnalysis&) = delete;
    WordAnalysis& operator=(WordAnalysis&) = delete;

    static WordAnalysis& getInstance() {
        static WordAnalysis ins;
        return ins;
    }

    const static std::vector<std::string> key;
    const static std::vector<std::string> bound;

    const static std::map<std::string, int>& get_key_map();
    const static std::map<std::string, int>& get_bound_map();


    bool process_key(std::string::iterator& iter, const std::string::iterator& end);
    bool process_id(std::string::iterator& iter, const std::string::iterator& end);
    bool process_str(std::string::iterator& iter, const std::string::iterator& end);
    bool process_char(std::string::iterator& iter, const std::string::iterator& end);
    bool process_constant(std::string::iterator& iter, const std::string::iterator &end);
    bool process_bound(std::string::iterator& iter, const std::string::iterator &end);

    void process_file(std::ifstream &file, bool print=false);
    TokenList get_tokenList() const;  //TODO: 引用TokenList

};

std::ostream& print_token(std::ostream& out, const TokenList& tk, long s, long e);


#endif //COMPLIE_WORDANALYSIS_H
