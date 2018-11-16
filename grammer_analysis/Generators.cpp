//
// Created by wmj on 18-11-15.
//

#include "Generators.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include "../Utility.h"

using namespace std;

Generators::Generators(const std::vector<std::string>& terminators,
                               const std::vector<std::string>& non_terminators):
    terminators(terminators), non_terminators(non_terminators)
{

}

void Generators::load_text(std::string name) {
    ifstream fin;
    fin.open(name);
    string buffer;
    getline(fin, buffer); // VN
    non_terminators = split(buffer, '`');
    getline(fin, buffer);
    terminators = split(buffer, '`');
    while (getline(fin, buffer)) {
        vector<string> gens = split(buffer, '`');
        string A = gens[0];
        for (int i = 1; i < gens.size(); i++) {
            auto B = split(gens[i]);
            add_generator(A, B);
        }
    }

}

std::vector<generator> Generators::get_generators(const generator_A& A) {
    return generators[A];
}

std::set<std::string> Generators::first(const std::string& A) {
    set<string> result;
    if (find(terminators.begin(), terminators.end(), A) != terminators.end())
        result.insert(A);
    else {
        // TODO: 优化求first集合的逻辑
        vector<generator> gens = generators[A];
        for (auto X : gens) {
            set<string> to_union = first(X.second[0]);
            for (const string &fxxkcpp: to_union)
                result.insert(fxxkcpp);
        }
    }
    return result;

}

bool Generators::isVN(const std::string& s) const {
    return find(non_terminators.begin(), non_terminators.end(), s) == non_terminators.end();
}

bool Generators::isVT(const std::string& s) const {
    return find(terminators.begin(), terminators.end(), s) == terminators.end();
}

Generators::Generators() = default;

void Generators::add_generator(const generator &g) {
    add_generator(g.first, g.second);
}

void Generators::add_generator(const generator_A &A, const generator_B& B) {
    if (generators.find(A) == generators.end())
        generators.emplace(A, vector<generator>());
    generators[A].emplace_back(A, B);
}

void Generators::show() {
    cout << "VT = {";
    for (auto x: terminators)
        cout << x << ", ";
    cout << "}\n";
    cout << "VN = {";
    for (auto x: non_terminators)
        cout << x << ", ";
    cout << "}\n";
    for (auto x: generators) {
        for (auto b: x.second) {
            cout << x.first << "  ->  ";
            for (const auto &c: b.second)
                cout <<  c;
            cout << endl;
        }
        cout << endl;
    }
}

int main_generators() {
     vector<string> a;
     Generators g(a, a);
     g.load_text("../grammer_analysis/expression_analytic");
}

generator_B make_generator_B(const std::string &s) {
    generator_B g;
    g.push_back(s);
    return g;
}

generator make_generator(const std::string &A, const std::string &B) {
    return make_pair(A, make_generator_B(B));
}
