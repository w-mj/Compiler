//
// Created by wmj on 18-11-15.
//

#include "LR1.h"
#include "LR1_DFA.h"
#include <iostream>

using namespace std;

int main() {
    Generators generators;
    generators.load_text("../grammar_analysis/expression_analytic");
    generators.show();

    LR1 lr1(generators);
    lr1.show();

}

LR1::LR1(Generators &g): generators(g) {
    LR1_DFA dfa(g);
    dfa.show();
    table = dfa.get_table();
}

void LR1::show() {
    for (int i = 0; i < table.size(); i++) {
        for (const auto &m: table[i]) {
            if (m.second.first != 'e') {
                cout << i << " " << m.first << "  : ";
                if (m.second.first == 'r') {
                    cout << "reduce: ";
                    show_generator(generators[m.second.second]);
                } else if (m.second.first == 's') {
                    cout << "shift in: " << m.second.second;
                }
                cout << endl;
            }
        }
    }
}
