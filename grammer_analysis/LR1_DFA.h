//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_LR1_DFA_H
#define COMPLIE_LR1_DFA_H
#include <string>
#include <map>
#include "Generators.h"


class LR1_DFA {
    struct LR1_Generator {
        generator gen;
        std::string suffix="#";
        int dot=0;
        LR1_Generator(const generator& g, const std::string& suffix="#", int dot=0);
        std::string get_suffix() const;
        std::string get_after_dot() const;
    };

    struct Node {
        std::vector<LR1_DFA::LR1_Generator> generator;
        std::map<std::string, Node*> transfer;
    };

    Node* root = nullptr;
    std::vector<Node*> all_nodes;
    void build(Generators& generators, std::string start);
};


#endif //COMPLIE_LR1_DFA_H
