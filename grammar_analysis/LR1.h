//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_LR1_H
#define COMPLIE_LR1_H
#include "LR1_DFA.h"
#include "Generators.h"
#include "../word_analysis/TokenList.h"

class LR1 {
    Generators generators;
    std::vector<std::vector<std::pair<char, size_t>>> table;
    std::map<std::string, size_t> index;
    TokenList& tokenList;
public:

    LR1(Generators& g, TokenList& tokenList);
    LR1(Generators& g, TokenList& tokenList, std::string fname);
    void show();
    bool process(TokenGetter& getter);
    void save(std::string fname);

    void build();
};


#endif //COMPLIE_LR1_H
