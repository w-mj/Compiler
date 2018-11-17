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
    const WordAnalysis& analyser;
public:

    LR1(Generators& g, const WordAnalysis& analyser);
    void show();
    bool process(TokenList::const_iterator& begin, const TokenList::const_iterator& end);
    bool process();
};


#endif //COMPLIE_LR1_H
