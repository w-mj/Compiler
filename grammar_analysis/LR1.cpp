//
// Created by wmj on 18-11-15.
//

#include "LR1.h"
#include "LR1_DFA.h"
#include "../Utility.h"
#include "GrammarTree.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stack>

using namespace std;

LR1::LR1(Generators &g, const WordAnalysis& analyser): generators(g), analyser(analyser) {
    LR1_DFA dfa(g);
    // dfa.show();
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
    of << "<table>\n";
    for (size_t i = 0; i < generators.size(); i++) {
        of << "<tr><td>" << i << "</td><td>:</td><td>" << generators[i].first << " --> ";
        for (const auto& x: generators[i].second)
            of << x;
        of << "</td></tr>\n";
    }
    of << "</table>\n";
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

bool LR1::process(TokenList::const_iterator &begin, const TokenList::const_iterator &end) {
    stack<generator_A> alpha_stack;
    stack<Node*> tree_node_stack;
    alpha_stack.push("#");
    tree_node_stack.push(new Node("#"));
    stack<size_t> state_stack;
    state_stack.push(0);
    TokenList::const_iterator& cursor = begin;
    try {
        while (true) {
            size_t state = state_stack.top();

            string alpha;
            string str;
            if (cursor == end) {
                alpha = "#";
                str = "#";
            }
            else if (cursor->first == 'c') {
                alpha = "i";  // 常数
                str = analyser.get_token_num(*cursor).str();
            }
            else if (cursor->first == 'p' && generators.isVT(analyser.get_token(*cursor))) {
                alpha = analyser.get_token(*cursor);
                str = alpha;
            }
            else
                throw runtime_error("Not support " + analyser.get_token(*cursor) + " yet.");

            const auto &action = table[state][alpha];
            switch (action.first) {
                case 's': {
                    state_stack.push(action.second);
                    alpha_stack.push(alpha);
                    tree_node_stack.push(new Node(str));
                    cursor++;
                    break;
                }
                case 'r': {
                    const auto &gen = generators[action.second];
                    if (gen.first == "@Start") {// acc
                        Node* n = tree_node_stack.top();
                        tree_node_stack.pop();
                        n->json();
                        delete_tree(n);
                        return true;
                    }
                    Node *new_node = new Node(gen.first);
                    for (auto i = 0; i < gen.second.size(); i++) {
                        state_stack.pop();
                        alpha_stack.pop();
                        new_node->children.insert(new_node->children.begin(), tree_node_stack.top());
                        tree_node_stack.pop();
                    }
                    alpha_stack.push(gen.first);
                    state_stack.push(table[state_stack.top()][gen.first].second);
                    tree_node_stack.push(new_node);
                    break;
                }
                default:
                    throw runtime_error("ERROR at LR1 analysis.");
            }
        }
    } catch(runtime_error& r) {
        while (!tree_node_stack.empty()) {
            delete_tree(tree_node_stack.top());
            tree_node_stack.pop();
        }
        throw;
    }
}

bool LR1::process() {
    const TokenList& tk = analyser.get_token_list();
    auto x = tk.begin();
    process(x, tk.cend());
}
