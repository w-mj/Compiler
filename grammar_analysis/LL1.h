//
// Created by wmj on 18-11-25.
//

#ifndef COMPLIE_LL1_H
#define COMPLIE_LL1_H
#include "Generators.h"
#include "../word_analysis/TokenList.h"

class LL1 {
    Generators gens;
    size_t **table;
    TokenList& tokenList;
    std::map<generator_A, size_t> nonter_map, ter_map;
public:
    LL1(Generators& generators, TokenList& tokenList);
    ~LL1();
    void build();

    void show();

    bool process(TokenGetter& getter);
};


#endif //COMPLIE_LL1_H
