//
// Created by wmj on 18-11-25.
//

#include "LL1.h"
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <fstream>
#include <iostream>

#include "../Utility.h"

using namespace std;

LL1::LL1(Generators &generators, TokenList &tokenList): gens(generators), tokenList(tokenList) {
    size_t ter_s = gens.terminator_size() + 1;
    size_t non_ter_s = gens.non_terminator_size();
    size_t max_size = gens.size();
    table = new size_t*[non_ter_s];
    for (size_t i = 0; i < non_ter_s; i++) {
        table[i] = new size_t[ter_s];
        for (size_t j = 0; j < ter_s; j++)
            table[i][j] = max_size;
    }

    size_t i = 0;
    for (const auto& A: gens.get_terminators())
        ter_map.emplace(A, i++);
    ter_map.emplace(gens.get_end(), i);
    i = 0;
    for (const auto& A: gens.get_non_terminators())
        nonter_map.emplace(A, i++);
}

void LL1::build() {
    vector<set<generator_A>> selects;
    selects.resize(gens.size());
    cout << "SELECTS: " << endl;

    for (size_t i = 0; i < gens.size(); i++) {
        if (gens.exists(i)) {
            generator gen = gens[i];
            set<generator_A> first = gens.first(gen.second[0]);
            if (in_set(first, gens.get_epsilon())) {
                first.erase(gens.get_epsilon());
                set<generator_A> follow = gens.follow(gen.first);
                selects[i].insert(follow.begin(), follow.end());
            }
            selects[i].insert(first.begin(), first.end());

            cout << i << "  " << gen << "         SELECT = " << selects[i] << endl;
        }
    }

    for (const auto& A: gens.get_non_terminators()) {
        set<generator_A> all_select_of_one;
        size_t cnt = 0;
        for (const auto i: gens.get_generators_index(A)) {
            cnt += selects[i].size();
            all_select_of_one.insert(selects[i].begin(), selects[i].end());
        }
        if (cnt != all_select_of_one.size())
            throw runtime_error("Conflict when build LL1 analysis table on " + A);
    }


    for (size_t i = 0; i < gens.size(); i++) {
        if (gens.exists(i)) {
            for (const auto& x: selects[i]) {
                table[nonter_map[gens[i].first]][ter_map[x]] = i;
            }
        }
    }
}

LL1::~LL1() {
    for (size_t i = 0; i < gens.non_terminator_size(); i++)
        delete[] table[i];
    delete[] table;
}

bool LL1::process(TokenGetter& getter) {
    stack<generator_A> st;
    st.push(gens.get_end());
    generator_A start = gens.get_start();

    st.push(start);


    while (!st.empty()) {
        generator_A top = st.top();
        st.pop();
        // cout << "st size:" << st.size() << " top: " << top << endl;

        string grammar_token = tokenList.get_grammar_token(getter.get());
        if (grammar_token == gens.get_end() && top == gens.get_end())
            break;

        if (gens.isVT(top)) {
            // cout << "   token:" << grammar_token << endl;
            if (top == grammar_token)
                if (getter.get().first != '#')
                    getter.next();
                else
                    throw runtime_error("input sequence finished but stack not empty");
            else
                throw runtime_error("Error in LL1 analysis (terminator not match).");
        } else if (gens.isVN(top)) {
            if (gens.exists(table[nonter_map[top]][ter_map[grammar_token]])) {
                generator gen = gens[table[nonter_map[top]][ter_map[grammar_token]]];
                // cout << "   input:" << grammar_token << "      use:" << gen << endl;
                for (auto it = gen.second.rbegin(); it != gen.second.rend(); it++) {
                    if (*it != gens.get_epsilon())
                        st.push(*it);
                }
            } else {
                throw runtime_error("Error in LL1 analysis (error in table)");
            }
        }
        else
            throw runtime_error("Internal Error. (neither VT nor VN)");
    }
    if (st.empty() && getter.get().first == '#')
        return true;
    throw runtime_error("process LL1 finish but stack is not empty or input not empty.");
}

void LL1::show() {
    auto alphabet = gens.get_terminators();
    auto temp = gens.get_non_terminators();
    ofstream of;
    of.open("LL1_table.html");
    of << "<html>\n";
    of << "<h1> LL1 Analysis Table.</h1>\n";
    of << "<p>VT = {";
    for (const auto &x: alphabet)
        of << x << ", ";
    of << "}</p>\n";
    of << "<p>VN = {";
    for (const auto &x: temp)
        of << x << ", ";
    of << "}</p>\n";
    of << "<table>\n";
    for (size_t i = 0; i < gens.size(); i++) {
        of << "<tr><td>" << i << "</td><td>:</td><td>" << gens[i].first << " --> ";
        for (const auto& x: gens[i].second)
            of << x;
        of << "</td></tr>\n";
    }
    of << "</table>\n";
    alphabet.emplace_back("#");

    of << "<table border='1'>\n";
    of << "<tr><td>&nbsp;</td>\n";
    for (const auto& alpha: alphabet)
        of << "<td>" << alpha << "</td>";
    of << "</tr>\n";

    for (size_t i = 0; i < temp.size(); i++) {
        of << "<tr><td>" << temp[i] << "</td>";
        for (size_t j = 0; j < ter_map.size(); j++) {
            const auto& m = table[i][j];
            if (table[i][j] == gens.size())
                of << "<td>&nbsp;</td>";
            else
                of << "<td>" << gens[table[i][j]] << "</td>";
        }
        of << "</tr>\n";
    }
    of << "</table>\n";
    of << "</html>\n";
    of.close();
}

