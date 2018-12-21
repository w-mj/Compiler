//
// Created by wmj on 18-11-15.
//

#include "LR1.h"
#include "LR1_DFA.h"
#include "../Utility.h"
#include "GrammarTree.h"
#include "../error/Error.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <memory>
#include <ctime>
#include <cassert>
#include <sstream>

using namespace std;

LR1::LR1(Generators &g, TokenList& tokenList): generators(g), tokenList(tokenList) {}

void LR1::show() {
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
            of << x << "&nbsp;";
        of << "</td></tr>\n";
    }
    of << "</table>\n";
    alphabet.emplace_back("#");
    alphabet.insert(alphabet.end(), temp.begin(), temp.end());

    of << "<table border='1'>\n";
    of << "<tr><td>&nbsp;</td>\n";

    vector<string> l(index.size());
    for (const auto& alpha: index)
        l[alpha.second] = alpha.first;

    for (const auto &i : l)
        of << "<td>" << i << "</td>";
    of << "</tr>\n";
    for (size_t i = 0; i < table.size(); i++) {
        of << "<tr><td>" << i << "</td>";
        for (size_t j = 0; j < l.size(); j++) {
            const auto& m = table[i][j];
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

bool LR1::process(TokenGetter& getter) {
    stack<generator_A> alpha_stack;
    stack<Node*> tree_node_stack;
    stack<size_t> state_stack;
    stack<void*> attr_stack;
    set<void*> to_delete;

    alpha_stack.push("#");
    tree_node_stack.push(new Node("#"));
    state_stack.push(0);

    try {
        while (true) {
            size_t state = state_stack.top();
            Token tk = getter.get();

            assert(tk.first != 'a');

            string alpha = tokenList.get_grammar_token(tk);
            string str = tokenList.get_token_str(tk);
            // cout << str << endl;

            const auto &action = table[state][index[alpha]];
            switch (action.first) {
                case 's': {
                    state_stack.push(action.second);
                    alpha_stack.push(alpha);
                    tree_node_stack.push(new Node(str));
                    void *v = nullptr;
                    if (generators.get_attr_builder() != nullptr) {
                        v = generators.get_attr_builder()(getter.get());
                        attr_stack.push(v);
                        to_delete.insert(v);
                    }
                    getter.next();
                    break;
                }
                case 'r': {
                    const auto &gen = generators[action.second];
                    if (gen.first == "@Start") {// acc
                        Node *n = tree_node_stack.top();
                        tree_node_stack.pop();
                        n->json();
                        delete_tree(n);
//                        for (auto x: to_delete)
//                            delete x;
                        return true;
                    }
                    // generators.get_attr(action.second)(nullptr);
                    bool attribute = generators.get_attr(action.second) != nullptr;
                    Node *new_node = new Node(gen.first);
                    vector<void *> attr_vec;
                    for (auto i = 0; i < gen.second.size(); i++) {
                        state_stack.pop();
                        alpha_stack.pop();
                        new_node->children.insert(new_node->children.begin(), tree_node_stack.top());
                        tree_node_stack.pop();
                        if (attribute) {
                            attr_vec.insert(attr_vec.begin(), attr_stack.top());
                            attr_stack.pop();
                        }
                    }
                    alpha_stack.push(gen.first);
                    state_stack.push(table[state_stack.top()][index[gen.first]].second);
                    tree_node_stack.push(new_node);
                    if (attribute) {
                        void* new_attr = generators.get_attr(action.second)(attr_vec);
                        attr_stack.push(new_attr);
                        to_delete.insert(new_attr);
                    }
                    break;
                }
                default:
                    cout << endl << endl;
                    cout << "alpha stack:  ";
                    while (!alpha_stack.empty()) {
                        cout << alpha_stack.top() << "  ";
                        alpha_stack.pop();
                    }
                    cout << endl;
                    cout << "state stack:  ";
                    while (!state_stack.empty()) {
                        cout << state_stack.top() << "  ";
                        state_stack.pop();
                    }
                    cout << endl;
                    cout << "current alpha:  " << alpha << endl;
                    stringstream error_msg;
                    error_msg << "Grammar ERROR: ";
                    error_msg << "expected ";
                    int k = 0;
                    for (size_t i = 0; i < table[state].size(); i++) {
                        auto& x = table[state][i];
                        if (x.first != 'e') {
                            error_msg << rindex[i] << " ";
                            k++;
                        }
                    }
                    error_msg << " before " << str;
                    error(error_msg.str());
                    throw runtime_error("ERROR at LR1 analysis. ");
                    break;
            }
        }
     }
   catch(runtime_error& r) {
        while (!tree_node_stack.empty()) {
            delete_tree(tree_node_stack.top());
            tree_node_stack.pop();
        }
       for (auto x: to_delete)
           delete x;
        throw;
    }
}

void LR1::save(std::string fname) {
    cout << "save LR1 table to \"" << fname << "\"" << endl;
    ofstream of(fname);
    for (auto &x: index) {
        of << x.first << " " << x.second << " ";
    }
    of << endl;
    for (auto &x: table) {
        for (auto &i: x) {
            of << i.first << i.second << " ";
        }
        of << endl;
    }
    of.close();
}

void LR1::build(bool show_dfa) {
    time_t timer = time(nullptr);
    LR1_DFA dfa(generators);
    timer = time(nullptr) - timer;
    cout << "building LR1 table finish. cost " << timer << " seconds." << endl;
    if (show_dfa)
        dfa.show();
    table = dfa.get_table(index);
    for (auto i: index) {
        rindex.emplace(i.second, i.first);
    }
}

void LR1::load(std::string fname) {
    cout << "load LR1 table from \"" << fname << "\"" << endl;
    generators.insert_nonterminators("@Start");
    generator expend_generator = make_generator("@Start", generators.get_start());
    size_t i0 = generators.add_generator(expend_generator);

    ifstream f(fname);
    string buffer;
    getline(f, buffer);
    auto index_vec = split(buffer);
    for (size_t i = 0; i < index_vec.size(); i += 2) {
        index.emplace(index_vec[i], stoi(index_vec[i + 1]));
        rindex.emplace(stoi(index_vec[i + 1]), index_vec[i]);
    }
    int cnt = 0;
    while (getline(f, buffer)) {
        table.emplace_back();
        auto line_vec = split(buffer);
        for (auto &x: line_vec) {
            string num(x.begin() + 1, x.end());
            table[cnt].emplace_back(x[0], stoi(num));
        }
        cnt += 1;
    }
    cout << "load finish. " << cnt << " states altogether." << endl;
}


