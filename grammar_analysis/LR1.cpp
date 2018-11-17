//
// Created by wmj on 18-11-15.
//

#include "LR1.h"
#include "LR1_DFA.h"
#include "../Utility.h"
#include <iostream>
#include <fstream>
#include <stack>

using namespace std;

int main() {
    Generators generators;
    generators.load_text("../grammar_analysis/test");
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
    auto alphabet = generators.get_terminators();
    auto temp = generators.get_non_terminators();
    ofstream of;
    of.open("LR1_table.html");
    of << "<html>\n";
    of << "<h1> LR1 Analysis Table.</h1>\n";
    of << "<p>VT = {";
    for (const auto &x: alphabet)
        of << x << ", ";
    of << "}</p>\n";
    of << "<p>VN = {";
    for (const auto &x: temp)
        of << x << ", ";
    of << "}</p>\n";
    for (size_t i = 0; i < generators.size(); i++) {
        of << "<p>" << i << ": " << generators[i].first << " --> ";
        for (const auto& x: generators[i].second)
            of << x;
        of << "</p>" << endl;
    }
    alphabet.emplace_back("#");
    alphabet.insert(alphabet.end(), temp.begin(), temp.end());

    of << "<table border='1'>\n";
    of << "<tr><td>&nbsp;</td>\n";
    for (const auto& alpha: alphabet)
        of << "<td>" << alpha << "</td>";
    of << "</tr>\n";
    for (size_t i = 0; i < table.size(); i++) {
        of << "<tr><td>" << i << "</td>";
        for (const auto& alpha: alphabet) {
            const auto& m = table[i][alpha];
            if (m.first == 'e')
                of << "<td>&nbsp;</td>";
            else if (m.first == 'r')
                of << "<td>r" << m.second << "</td>";
            else if (m.first == 's')
                of << "<td>s" << m.second << "</td>";
        }
        of << "</tr>\n";
    }
    of << "</table>\n";
    of << "</html>\n";
    of.close();
}

bool LR1::process(TokenList::iterator &begin, TokenList::iterator &end) {
    size_t state = 0;
    stack<generator_A> st;
}
