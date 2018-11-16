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
    getline(fin, start);
    while (getline(fin, buffer)) {
        vector<string> gens = split(buffer, '`');
        string A = gens[0];
        if (!isVN(A))
            throw runtime_error(A + " is not a non terminal.");
        for (int i = 1; i < gens.size(); i++) {
            auto B = split(gens[i]);
            for (const string& s: B)
                if (!(isVN(s) || isVT(s)))
                    throw runtime_error(s + " is not in alphabet.");
            add_generator(A, B);
        }
    }

    build_first_set();

}

std::vector<size_t> Generators::get_generators_index(const generator_A& A) {
    return g_map[A];
}

std::set<std::string> Generators::first(const std::string& A) {
    if (isVT(A))
        return {A};
    else first_set[A];

}

bool Generators::isVN(const std::string& s) const {
    return find(non_terminators.begin(), non_terminators.end(), s) != non_terminators.end();
}

bool Generators::isVT(const std::string& s) const {
    return find(terminators.begin(), terminators.end(), s) != terminators.end();
}

Generators::Generators() = default;

size_t Generators::add_generator(const generator &g) {
    if (g_map.find(g.first) == g_map.end())
        g_map.emplace(g.first, vector<size_t>());
    size_t i = g_list.size();
    g_map[g.first].emplace_back(i);
    g_list.push_back(g);
    return i;
}

size_t Generators::add_generator(const generator_A &A, const generator_B& B) {
    return add_generator(make_pair(A, B));
}

void Generators::show() {
    cout << "VT = {";
    for (const auto &x: terminators)
        cout << x << ", ";
    cout << "}\n";
    cout << "VN = {";
    for (const auto &x: non_terminators)
        cout << x << ", ";
    cout << "}\n";
    for (const auto &x: g_map) {
        for (const auto& g: x.second)
            show_generator(g_list[g]);
        cout << endl;
    }
}

generator_A Generators::get_start() const {
    return start;
}

std::vector<generator_A> Generators::get_non_terminators() {
    return non_terminators;
}

std::vector<generator_A> Generators::get_terminators() {
    return terminators;
}

const generator &Generators::operator[](size_t i) {
    return g_list[i];
}

void Generators::build_first_set() {
    first_set.clear();
    for (const auto& s: non_terminators)
        first_set.emplace(s, set<string>());
    bool quit = false;
    int cnt = 0;
    while (!quit) {
        quit = true;
        for (const auto& s: non_terminators) {
            vector<size_t> gens = get_generators_index(s);
            for (size_t i: gens) {
                const generator &gen = operator[](i);
                if (isVT(gen.second[0])) {
                    if (insert_set(first_set[s], gen.second[0]))
                        quit = false;
                } else {
                    if (union_set(first_set[s], first_set[gen.second[0]]))
                        quit = false;
                }
            }
        }
    }
}

generator_B make_generator_B(const std::string &s) {
    generator_B g;
    g.push_back(s);
    return g;
}

generator make_generator(const std::string &A, const std::string &B) {
    return make_pair(A, make_generator_B(B));
}

void show_generator(const generator& g) {
    cout << g.first << "  ->  ";
    for (const auto &c: g.second)
        cout <<  c;
    cout << endl;
}