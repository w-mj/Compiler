//
// Created by wmj on 18-11-15.
//

#include "LR1_DFA.h"
#include <queue>
#include <utility>
#include <vector>
#include <iostream>
#include "../Utility.h"

using namespace std;


/**
 * 通过产生式构建活前缀的DFA
 * @param generators 产生式集合
 * @param start 开始符号
 */
void LR1_DFA::build() {
    root = new Node();
    all_nodes.push_back(root);
    root->generator_list.emplace_back(make_generator("@Start", generators.get_start()));

    queue<LR1_Generator> to_extend;  // 待扩展生成式

    for (size_t _ = 0; _ < all_nodes.size(); _++) {
        Node *current_node = all_nodes[_];
        // 扩展当前节点的主产生式
        for (const LR1_Generator& main_gen: current_node->generator_list)
            to_extend.push(main_gen);
        while (!to_extend.empty()) {
            LR1_Generator expend = to_extend.front();
            to_extend.pop();

            const string &after_dot = expend.get_after_dot();  // 点后面的第一个符号
            if (generators.isVN(after_dot)) {
                // 扩展点后面的第一个非终结符
                vector<generator> extended_generators = generators.get_generators(after_dot);  // 找到点后非终结符的所有产生式
                for (const generator &gen: extended_generators) {
                    string outlook_V = expend.get_outlook();
                    set<string> outlook_set;
                    if (outlook_V == "#") {  // 点后面只有一个非终结符，继承展望集
                        outlook_set.insert(expend.outlook);
                    } else {  // 否则展望集为点后面的后面的符号的first
                        set<string> first_outlook_VN = generators.first(outlook_V);
                        outlook_set.insert(first_outlook_VN.begin(), first_outlook_VN.end());
                    }
                    for (const string &outlook: outlook_set) {
                        LR1_Generator new_generator(gen, outlook, 0);
                        if (!in_vec(current_node->generator_list, new_generator)) {  // 如果该节点未被添加过
                            current_node->generator_list.push_back(new_generator);
                            to_extend.push(new_generator);
                        }
                    }
                }
            }
        }
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
            for (const LR1_Generator &gen: suffix_item.second) {
                // 为后继节点添加主产生式
                new_node->generator_list.emplace_back(gen.gen, gen.outlook, gen.dot + 1);
            }
            vector<Node *>::const_iterator n;
            for (n = all_nodes.begin(); n != all_nodes.end(); n++) {
                if ((*n)->equal(*new_node))
                    break;
            }
            if (n == all_nodes.end()) {
                new_node->index = all_nodes.size();
                all_nodes.push_back(new_node);
            } else {
                delete new_node;
                new_node = *n;
            }
            current_node->transfer.emplace(suffix_item.first, new_node);
        }
    }
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

vector<map<string, LR1_DFA::TableStatus>> LR1_DFA::get_table() {
    map<string, LR1_DFA::TableStatus> init;
    for (auto x: generators.get_terminators())
        init.emplace(x, TableStatus('e', 0));
    init.emplace("#", TableStatus('e', 0));
    for (auto x: generators.get_non_terminators())
        init.emplace(x, TableStatus('e', 0));

    vector<map<string, LR1_DFA::TableStatus>> result(all_nodes.size());

    for (size_t i = 0; i < all_nodes.size(); i++) {
        Node* cn = all_nodes[i];
        for (const LR1_Generator& gen: cn->generator_list) {
            if (gen.dot == gen.gen.second.size())
                if (result[i][gen.outlook].status == 'e')
                    result[i][gen.outlook] = TableStatus('r', gen.gen);
                else
                    throw runtime_error("Conflict when reduction at status I" + to_string(i));
        }
        for (const auto& t: cn->transfer) {
            if (result[i][t.first].status == 'e')
                result[i][t.first] = TableStatus('s', t.second->index);
            else
                throw runtime_error("Conflict when shift in at status I" + to_string(i));
        }
    }
    return result;
}


std::string LR1_DFA::LR1_Generator::get_after_dot() const {
    if (dot < gen.second.size())
        return gen.second[dot];
    else
        return "#";
}

std::string LR1_DFA::LR1_Generator::get_outlook() const {
    if (dot < gen.second.size() - 1)
        return gen.second[dot + 1];
    else
        return outlook;
}

LR1_DFA::LR1_Generator::LR1_Generator(const generator& g, const string &outlook, int dot)
        : gen(g), outlook(outlook), dot(dot) {

}

bool LR1_DFA::LR1_Generator::operator==(const LR1_DFA::LR1_Generator &another) const{
    return gen == another.gen && dot == another.dot && outlook == another.outlook;
}

void LR1_DFA::LR1_Generator::show() const {
    cout << gen.first << "  ->  ";
    for (size_t i = 0; i < gen.second.size(); i++) {
        if (i == dot)
            cout << '`';
        cout << gen.second[i];
    }
    if (dot == gen.second.size())
        cout << '`';
    cout << "  ," << outlook << endl;
}

bool LR1_DFA::Node::operator==(const LR1_DFA::Node &another) const {
    return generator_list == another.generator_list;
}

bool LR1_DFA::Node::equal(const LR1_DFA::Node &another) const {
    for (const LR1_Generator& gen: another.generator_list) {
        if (!in_vec(generator_list, gen))  // A 不完全包含 B
            break;
        return true;
    }
    for (const LR1_Generator& gen: generator_list) {
        if (!in_vec(another.generator_list, gen))
            break;
        return true;
    }
    return false;
}

LR1_DFA::TableStatus::TableStatus(char s, generator g) {
    status = s;
    data.gen = g;
}

LR1_DFA::TableStatus::TableStatus(char s, size_t j) {
    status = s;
    data.jump = j;
}
