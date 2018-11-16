//
// Created by wmj on 18-11-15.
//

#include "LR1.h"

LR1::LR1() = default;

void LR1::generate_table(void) {

}

int main() {
    Generators generators;
    generators.load_text("../grammer_analysis/test");
    generators.show();

    LR1_DFA dfa(generators, "S");
    dfa.show();

}