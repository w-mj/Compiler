//
// Created by wmj on 18-11-27.
//

#ifndef COMPLIE_GRAMMAR_H
#define COMPLIE_GRAMMAR_H

#include "Generators.h"
#include "../word_analysis/TokenList.h"

class Grammar {
public:
    static Generators Basic_Exp();
    static Generators Basic_Exp_Quat(TokenList& tokenList);
    static Generators C_Exp();
    static Generators C_Bin_Exp();

    static Generators C_Grammar();
};


#endif //COMPLIE_GRAMMAR_H
