//
// Created by wmj on 18-11-25.
//

#include "LL1.h"
#include <vector>
#include <map>
#include <set>
#include <stack>
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
    for (size_t i = 0; i < gens.terminator_size(); i++)
        delete[] table[i];
    delete[] table;
}

bool LL1::process() {
    auto x = tokenList.begin();
    return process(x, tokenList.end(), gens.get_start());
}

bool LL1::process(TokenList::iterator &begin, const TokenList::iterator &end, const generator_A &start) {
    stack<generator_A> st;
    st.push(gens.get_end());
    st.push(start);
    auto cursor = begin;
    while (!st.empty()) {
        generator_A top = st.top();
        st.pop();

        if (gens.isVT(top)) {
            if (top == tokenList.get_grammar_token(cursor, end))
                cursor++;
            else
                throw runtime_error("Error in LL1 analysis (terminator not match).");
        } else if (gens.isVN(top)) {
            if (gens.exists(table[nonter_map[top]][ter_map[tokenList.get_grammar_token(cursor, end)]])) {
                generator gen = gens[table[nonter_map[top]][ter_map[tokenList.get_grammar_token(cursor, end)]]];
                for (auto it = gen.second.rbegin(); it != gen.second.rend(); it++) {
                    st.push(*it);
                }
            } else {
                throw runtime_error("Error in LL1 analysis (error in table)");
            }
        }
    }
    return true;
}

