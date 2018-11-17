//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_LR1_H
#define COMPLIE_LR1_H
#include "LR1_DFA.h"
#include "Generators.h"
#include "../word_analysis/WordAnalysis.h"

class LR1 {
    Generators &generators;
    std::vector<std::map<std::string, std::pair<char, size_t>>> table;
public:
    explicit LR1(Generators& g);
    void show();
    bool process(TokenList::iterator& begin, TokenList::iterator& end);
};


#endif //COMPLIE_LR1_H
