//
// Created by wmj on 18-11-15.
//

#include "Generators.h"
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include "../Utility.h"

using namespace std;

void Generators::load_text(std::string name) {
    ifstream fin;
    fin.open(name);
    string buffer;
    getline(fin, buffer); // VN
    auto non_ter = split(buffer, '`');
    non_terminators.insert(non_ter.begin(), non_ter.end());
    getline(fin, buffer);
    auto ter = split(buffer, '`');
    terminators.insert(ter.begin(), ter.end());
    getline(fin, start);
    while (getline(fin, buffer)) {
        vector<string> gens = split(buffer, '`');
        string A = gens[0];
        if (!isVN(A))
            throw runtime_error(A + " is not a non terminal.");
        for (int i = 1; i < gens.size(); i++) {
            auto B = split(gens[i]);
            for (const string &s: B)
                if (!(isVN(s) || isVT(s)))
                    throw runtime_error(s + " is not in alphabet.");
            add_generator(A, B);
        }
    }
}

std::vector<size_t> Generators::get_generators_index(const generator_A &A) {
    return g_map[A];
}

std::set<std::string> Generators::first(const std::string &A) {
    if (!built_first) {
        build_first_set();
    }
    if (isVT(A) || A == epsilon)
        return {A};
    else return first_set[A];
}

bool Generators::isVN(const std::string &s) const {
    return find(non_terminators.begin(), non_terminators.end(), s) != non_terminators.end();
}

bool Generators::isVT(const std::string &s) const {
    return find(terminators.begin(), terminators.end(), s) != terminators.end();
}

Generators::Generators() {
    adder.gens = this;
}

size_t Generators::add_generator(const generator &g) {
    if (!in_vec(non_terminators, g.first))
        throw runtime_error(g.first + " is not a non-terminator.");
    for (const auto&x : g.second)
        if (!(in_vec(non_terminators, x) || in_vec(terminators, x) || x == epsilon))
            throw runtime_error(x + " is neither a terminator nor a non-terminator.");
    if (g_map.find(g.first) == g_map.end())
        g_map.emplace(g.first, vector<size_t>());
    size_t i = g_list.size();
    g_map[g.first].emplace_back(i);
    g_list.push_back(g);
    attr_list.push_back(nullptr);
    return i;
}

size_t Generators::add_generator(const generator_A &A, const generator_B &B) {
    return add_generator(make_pair(A, B));
}

void Generators::show() {
    cout << to_str();
}

generator_A Generators::get_start() const {
    return start;
}

vector<string> Generators::get_non_terminators() {
    return vector<string>(non_terminators.begin(), non_terminators.end());
}

vector<generator_A> Generators::get_terminators() {
    return vector<string>(terminators.begin(), terminators.end());
}

const generator &Generators::operator[](size_t i) {
    if (g_list[i].first == deleted)
        throw runtime_error(to_string(i) + "th generator has been deleted.");
    return g_list[i];
}

std::string Generators::to_str() {
    ostringstream ss;
    ss << "VT = {";
    for (const auto &x: terminators)
        ss << x << ", ";
    ss << "}\n";
    ss << "VN = {";
    for (const auto &x: non_terminators)
        ss << x << ", ";
    ss << "}\n";
    for (size_t i = 0; i < g_list.size(); i++) {
        ss << i << ": " << g_list[i].first << " --> ";
        for (const auto &x: g_list[i].second)
            ss << x << " ";
        ss << endl;
    }
    return ss.str();
}

size_t Generators::size() const {
    return g_list.size();
}

void Generators::build_first_set() {
    first_set.clear();
    for (const auto &s: non_terminators)
        first_set.emplace(s, set<string>());
    bool quit = false;
    while (!quit) {
        quit = true;
        for (const auto &s: non_terminators) {
            vector<size_t> gens = get_generators_index(s);
            for (size_t i: gens) {
                const generator &gen = operator[](i);
                if (isVT(gen.second[0]) || gen.second[0] == epsilon) {
                    if (insert_set(first_set[s], gen.second[0]))
                        quit = false;
                } else {
                    if (union_set(first_set[s], first_set[gen.second[0]]))
                        quit = false;
                }
            }
        }
    }
    built_first = true;
}

void Generators::build_follow_set() {
    follow_set.clear();
    for (const auto &s: non_terminators)
        follow_set.emplace(s, set<string>());
    follow_set[start].insert("#");
    for (const generator &gen: g_list) {
        for (size_t i = 0; i < gen.second.size() - 1; i++) {
            // A -> Bc
            if (in_vec(non_terminators, gen.second[i]) && in_vec(terminators, gen.second[i + 1]))
                follow_set[gen.second[i]].insert(gen.second[i + 1]);
        }
    }
    bool quit = false;
    while (!quit) {
        quit = true;
        for (const generator &gen: g_list) {
            // A -> B
            if (in_vec(non_terminators, last_vec(gen.second)))
                if (union_set(follow_set[last_vec(gen.second)], follow_set[gen.first]))
                    quit = false;
            // A -> BC  (C's first contains epsilon)
            for (size_t i = 0; i < gen.second.size() - 1; i++) {
                if (in_vec(non_terminators, gen.second[i]) && in_vec(non_terminators, gen.second[i + 1])) {
                    follow_set[gen.second[i]].insert(first_set[gen.second[i + 1]].begin(),
                                                     first_set[gen.second[i + 1]].end());
                    if (in_set(first_set[gen.second[i + 1]], epsilon)) {
                        follow_set[gen.second[i]].erase(epsilon);
                        if (union_set(follow_set[gen.second[i]], follow_set[gen.second[i + 1]]))
                            quit = false;
                    }
                }
            }
        }
    }
    built_follow = true;
}

std::set<std::string> Generators::follow(const std::string &B) {
    if (!built_follow) {
        build_follow_set();
    }
    if (isVN(B))
        return follow_set[B];
    throw runtime_error("Non terminators don't have follow set");
}

void Generators::remove_left_recursive() {
    vector<generator_A> non_ters_list(non_terminators.begin(), non_terminators.end());
    map<generator_A, size_t> priority;
    for (size_t i = 0; i < non_ters_list.size(); i++)
        priority.emplace(non_ters_list[i], i);
    bool quit = false;
    // 消除间接左递归
    for (const auto &i : non_ters_list) {
        // 将非终结符排序
        while (!quit) {
            quit = true;
            vector<size_t> new_index;
            for (auto it = g_map[i].begin(); it != g_map[i].end();) {
                // 遍历一个非终结符推导出的所有产生式
                size_t cur_gen_id = *it;
                if (in_vec(non_terminators, g_list[cur_gen_id].second[0]) &&
                    priority[g_list[cur_gen_id].first] > priority[g_list[cur_gen_id].second[0]]) {
                    quit = false;
                    for (size_t sub_gen: g_map[g_list[cur_gen_id].second[0]]) {
                        generator_B new_genB(g_list[sub_gen].second.begin(), g_list[sub_gen].second.end());
                        new_genB.insert(new_genB.end(), g_list[cur_gen_id].second.begin() + 1,
                                        g_list[cur_gen_id].second.end());
                        // add_generator(g_list[cur_gen_id].first, new_genB);
                        new_index.emplace_back(g_list.size());
                        // g_map[i].emplace_back(g_list.size());
                        g_list.emplace_back(g_list[cur_gen_id].first, new_genB);
                    }
                    g_list[cur_gen_id].first = deleted;
                    it = g_map[i].erase(it);
                } else
                    it++;  // 循环中删除
            }
            for (auto x: new_index)
                g_map[i].emplace_back(x);
        }



        // 消除直接左递归
        bool left_recursive = false;
        for (size_t cur_gen = 0; cur_gen < g_map[i].size() && !left_recursive; cur_gen++)
            if (g_list[g_map[i][cur_gen]].first == g_list[g_map[i][cur_gen]].second[0])
                left_recursive = true;
        if (left_recursive) {
            generator_A new_A = i + "'";
            vector<size_t> remove_list;
            for (unsigned long cur_gen_id : g_map[i]) {
                if (g_list[cur_gen_id].first == g_list[cur_gen_id].second[0]) {
                    g_list[cur_gen_id].first = new_A;
                    g_list[cur_gen_id].second.erase(g_list[cur_gen_id].second.begin());

                    remove_list.push_back(cur_gen_id);
                }
                g_list[cur_gen_id].second.push_back(new_A);
            }
            non_terminators.insert(new_A);
            add_generator(make_generator(new_A, epsilon));
            for (auto cur_gen_id: remove_list) {
                g_map[i].erase(find(g_map[i].begin(), g_map[i].end(), cur_gen_id));
                if (g_map.find(new_A) == g_map.end())
                    g_map.emplace(new_A, vector<size_t>());
                g_map[new_A].push_back(cur_gen_id);
            }
        }
    }

    built_follow = built_first = false;
}

void Generators::remove_generator(const generator_A &A, size_t n) {
    get_gen(A, n).first = deleted;
    g_map[A].erase(g_map[A].begin() + n);
}

void Generators::_print_first() {
    if (!built_first) {
        build_first_set();
    }
    for (const auto& E: non_terminators) {
        cout << "First("<<E<<") = " << first_set[E] <<endl;
    }
}

void Generators::_print_follow() {
    if (!built_follow) {
        build_follow_set();
    }
    for (const auto& E: non_terminators) {
        cout << "Follow("<<E<<") = {";
        for (const auto& M: follow_set[E])
            cout << M << ", ";
        cout << "}"<<endl;
    }
}

bool Generators::exists(size_t t) {
    return t < size() && g_list[t].first != deleted;
}

generator_A Generators::get_end() const {
    return end;
}

generator_A Generators::get_epsilon() const {
    return epsilon;
}

size_t Generators::non_terminator_size() const {
    return non_terminators.size();
}

size_t Generators::terminator_size() const {
    return terminators.size();
}

generator_B make_generator_B(const std::string &s) {
    generator_B g;
    g.push_back(s);
    return g;
}

generator make_generator(const std::string &A, const std::string &B) {
    return make_pair(A, make_generator_B(B));
}

void show_generator(const generator &g) {
    cout << g;
}

std::ostream& operator<<(std::ostream& out, const generator &g) {
    out << g.first << "  ->  ";
    for (const auto &c: g.second)
        out << c;
    return out;
}

Generators::GeneratorAdder &Generators::GeneratorAdder::operator<<(const generator_B &B) {
    last_index = gens->size();
    gens->add_generator(A, B);
    return *this;
}

Generators::GeneratorAdder &Generators::GeneratorAdder::operator<<(const std::string &B) {
    return operator<<(split(B));
}

Generators::GeneratorAdder &Generators::GeneratorAdder::operator|(const std::string &B) {
    return operator<<(split(B));
}

Generators::GeneratorAdder &Generators::GeneratorAdder::operator>>(const std::string &B) {
    return operator<<(split(B));
}

Generators::GeneratorAdder &Generators::GeneratorAdder::operator|(Attribute attr) {
    gens->set_attr(last_index, attr);
    return *this;
}

Generators::GeneratorAdder &Generators::add(const generator_A &A) {
    adder.A = A;
    return adder;
}

void Generators::set_terminators(const std::set<generator_A> &s) {
    terminators.clear();
    terminators.insert(s.begin(), s.end());
}

void Generators::set_non_terminators(const set<generator_A>& s) {
    non_terminators.clear();
    non_terminators.insert(s.begin(), s.end());
}

void Generators::insert_nonterminators(const generator_A& n) {
    non_terminators.insert(n);
}

void Generators::set_start(const generator_A& s) {
    start = s;
}

void Generators::set_terminators(const std::string &s) {
    auto v = split(s);
    set_terminators(set<string>(v.begin(), v.end()));
}

void Generators::set_non_terminators(const std::string &s) {
    auto v = split(s);
    set_non_terminators(set<string>(v.begin(), v.end()));
}

Generators::GeneratorAdder &Generators::operator<<(const generator_A &A) {
    adder.A = A;
    return adder;
}

Generators::~Generators() {
}

void Generators::set_attr(size_t i, Attribute attr) {
    attr_list[i] = attr;
}

Attribute Generators::get_attr(size_t i) {
    if (attr_list[i] != nullptr)
        return attr_list[i];
    return do_nothing;
}

void Generators::set_attr_builder(AttrBuilder attr) {
    attr_builder = attr;
}

AttrBuilder Generators::get_attr_builder() {
    return attr_builder;
}

const generator *Generators::get_addr(size_t i) const {
    return &g_list[i];
}
