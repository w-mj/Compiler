//
// Created by wmj on 18-11-27.
//

#ifndef COMPLIE_GRAMMAR_H
#define COMPLIE_GRAMMAR_H

#include "Generators.h"

class Grammar {
public:
    static Generators Basic_Exp();
    static Generators C_Exp();
    static Generators C_Grammar();
};


#endif //COMPLIE_GRAMMAR_H
