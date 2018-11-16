//
// Created by wmj on 18-11-15.
//

#include "LR1.h"
#include "LR1_DFA.h"
#include <iostream>

using namespace std;

int main() {
    Generators generators;
    generators.load_text("../grammar_analysis/test");
    generators.show();

    LR1 lr1(generators);
    lr1.show();

}

LR1::LR1(Generators &g) {
    LR1_DFA dfa(g);
    dfa.show();
    table = dfa.get_table();
}

void LR1::show() {
    for (int i = 0; i < table.size(); i++) {
        for (const auto &m: table[i]) {
            if (m.second.status != 'e') {
                cout << i << " " << m.first << "  : ";
                if (m.second.status == 'r') {
                    cout << "reduce: ";
                    show_generator(m.second.data.gen);
                } else if (m.second.status == 's') {
                    cout << "shift in: " << m.second.data.jump;
                }
                cout << endl;
            }
        }
    }
}
