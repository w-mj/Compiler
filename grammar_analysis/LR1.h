//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_LR1_H
#define COMPLIE_LR1_H
#include "LR1_DFA.h"
#include "Generators.h"

class LR1 {
    std::vector<std::map<std::string, LR1_DFA::TableStatus>> table;
public:
    explicit LR1(Generators& g);
    void show();
};


#endif //COMPLIE_LR1_H
