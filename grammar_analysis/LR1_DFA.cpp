//
// Created by wmj on 18-11-15.
//

#include "LR1_DFA.h"
#include <queue>
#include <utility>
#include <vector>
#include <iostream>
#include <sstream>
#include "../Utility.h"

using namespace std;


/**
 * 通过产生式构建活前缀的DFA
 * @param generators 产生式集合
 * @param start 开始符号
 */
void LR1_DFA::build() {
    root = new Node();

    generators.insert_nonterminators("@Start");
    generator expend_generator = make_generator("@Start", generators.get_start());
    size_t i0 = generators.add_generator(expend_generator);
    // root->generator_list.emplace(generators.get_addr(i0), i0);
    root->main_generator_list.emplace(generators.get_addr(i0), i0);

    root->closure(generators);
    all_nodes.push_back(root);

    for (size_t i = 0; i < all_nodes.size(); i++) {
        double percent = ((i + 1.0) / all_nodes.size()) * 100;
        cout << "building LR1 DFA node " << i + 1 << "/" << all_nodes.size() << "  " << percent << "%\r" << flush;
        Node *current_node = all_nodes[i];

        // 开始生成后继节点
        // 根据点后面的符号生成索引，作为状态转换
        map<string, vector<LR1_Generator>> after_dot_set;
        for (const LR1_Generator &gen: current_node->generator_list) {
            const string &after_dot = gen.get_after_dot();
            if (after_dot != "#") {
                if (after_dot_set.find(after_dot) == after_dot_set.end())
                    after_dot_set.emplace(after_dot, vector<LR1_Generator>());
                after_dot_set[after_dot].push_back(gen);
            }
        }

        // 对每一个状态转换构建后继节点
        for (const auto &suffix_item: after_dot_set) {
            auto *new_node = new Node();
            // new_node->index = all_nodes.size();

            for (const LR1_Generator &gen: suffix_item.second) {
                // 为后继节点添加主产生式
                // new_node->generator_list.emplace(gen.gen, gen.gen_index, gen.outlook, gen.dot + 1);
                new_node->main_generator_list.emplace(gen.gen, gen.gen_index, gen.outlook, gen.dot + 1);
            }

            vector<Node *>::const_iterator n;
            for (n = all_nodes.begin(); n != all_nodes.end(); n++) {
                if ((*n)->operator==(*new_node))
                    break;
            }
            if (n == all_nodes.end()) {
                new_node->closure(generators); // 为新节点计算闭包
                new_node->index = all_nodes.size();
                all_nodes.push_back(new_node);
            } else {
                delete new_node;
                new_node = *n;
            }
            current_node->transfer.emplace(suffix_item.first, new_node);
        }
    }
    cout << endl << "LR1 table " << all_nodes.size() << " states." << endl;
}

LR1_DFA::LR1_DFA(Generators &generators): generators(generators) {
    build();
}

LR1_DFA::~LR1_DFA() {
    for (auto x: all_nodes)
        delete x;
}

void LR1_DFA::show() {
    for (size_t i = 0; i < all_nodes.size(); i++) {
        cout << "I" << i << ": " << endl;
        for (const LR1_Generator& g: all_nodes[i]->generator_list) {
            g.show();
        }
        cout << "----------" << endl;
        for (const auto &t: all_nodes[i]->transfer) {
            cout << t.first << "  ~~~~~>  I" <<
                 find(all_nodes.begin(), all_nodes.end(), t.second) - all_nodes.begin() << endl;
        }
        cout << "==========" << endl;
    }
}

vector<vector<pair<char, size_t>>> LR1_DFA::get_table(map<string, size_t>& index) {
    index.clear();
    size_t cnt = 0;
    for (const auto &x: generators.get_terminators()) {
        index.emplace(x, cnt++);
    }
    index.emplace("#", cnt++);

    for (auto x: generators.get_non_terminators()) {
        if (x != "@Start")
            index.emplace(x, cnt++);
    }


    vector<vector<pair<char, size_t>>> new_table(all_nodes.size(), vector<pair<char, size_t>>(cnt, {'e', 0}));


    for (size_t i = 0; i < all_nodes.size(); i++) {
        Node* cn = all_nodes[i];
        for (const LR1_Generator& gen: cn->generator_list) {
            if (gen.dot == gen.gen->second.size())
                if (new_table[i][index[gen.outlook]].first == 'e') {
                    new_table[i][index[gen.outlook]].first = 'r';
                    new_table[i][index[gen.outlook]].second = gen.gen_index;
                }
                else
                    throw runtime_error("Conflict when reduction at status I" + to_string(i));
        }
        for (const auto& t: cn->transfer) {
            if (new_table[i][index[t.first]].first != 'e') {
                cout << "A Shift/Reduction confilict occurs at I" << to_string(i) << " select shift as default." << endl;
            }
            new_table[i][index[t.first]].first = 's';
            new_table[i][index[t.first]].second = t.second->index;
        }
    }
    return new_table;
}


std::string LR1_DFA::LR1_Generator::get_after_dot() const {
    if (dot < gen->second.size())
        return gen->second[dot];
    else
        return "#";
}

std::string LR1_DFA::LR1_Generator::get_outlook() const {
    if (dot < gen->second.size() - 1)
        return gen->second[dot + 1];
    else
        return outlook;
}

LR1_DFA::LR1_Generator::LR1_Generator(const generator *g, size_t index, const string &outlook, int dot)
        : gen(g), gen_index(index), outlook(outlook), dot(dot) {
    s = str();
}

bool LR1_DFA::LR1_Generator::operator==(const LR1_DFA::LR1_Generator &another) const{
    return gen_index == another.gen_index && dot == another.dot && outlook == another.outlook;
}

void LR1_DFA::LR1_Generator::show() const {
    cout << gen->first << "  ->  ";
    for (size_t i = 0; i < gen->second.size(); i++) {
        if (i == dot)
            cout << '`';
        cout << gen->second[i];
    }
    if (dot == gen->second.size())
        cout << '`';
    cout << "  ," << outlook << endl;
}

bool LR1_DFA::LR1_Generator::operator<(const LR1_DFA::LR1_Generator &another) const {
    return str() < another.str();
}

std::string LR1_DFA::LR1_Generator::str() const {
    if (s.empty()) {
        stringstream ss;
        ss << gen->first;
        for (size_t i = 0; i < gen->second.size(); i++) {
            if (i == dot)
                ss << '*';
            ss << gen->second[i];
        }
        ss << outlook;
        return ss.str();
    } else
        return s;
}


bool LR1_DFA::Node::operator==(const LR1_DFA::Node &another) const {
    return main_generator_list == another.main_generator_list;
}

void LR1_DFA::Node::closure(Generators &generators) {
    queue<LR1_Generator> to_extend;  // 待扩展生成式
    // 扩展当前节点的主产生式
    for (const LR1_Generator& main_gen: main_generator_list) {
        to_extend.push(main_gen);
        generator_list.emplace(main_gen);
    }


    while (!to_extend.empty()) {
        auto expend = to_extend.front();
        to_extend.pop();

        const string &after_dot = expend.get_after_dot();  // 点后面的第一个符号
        if (generators.isVN(after_dot)) {
            // 扩展点后面的第一个非终结符
            vector<size_t> extended_generators = generators.get_generators_index(after_dot);  // 找到点后非终结符的所有产生式
            for (size_t i: extended_generators) {
                // auto& gen = generators[i];
                string outlook_V = expend.get_outlook();
                set<string> outlook_set;
                if (outlook_V == "#") {  // 点后面只有一个非终结符，继承展望集
                    outlook_set.insert(expend.outlook);
                } else {  // 否则展望集为点后面的后面的符号的first
                    set<string> first_outlook_VN = generators.first(outlook_V);
                    if (in_set(first_outlook_VN, generators.get_epsilon()))
                        outlook_set.insert(expend.outlook);
                    outlook_set.insert(first_outlook_VN.begin(), first_outlook_VN.end());
                }
                for (const string &outlook: outlook_set) {
                    LR1_Generator new_generator(generators.get_addr(i), i, outlook, 0);
                    if (!in_vec(generator_list, new_generator)) {  // 如果该节点未被添加过
                        generator_list.emplace(new_generator);
                        to_extend.push(new_generator);
                    }
                }
            }
        }
    }
}
