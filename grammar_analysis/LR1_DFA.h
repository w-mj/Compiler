//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_LR1_DFA_H
#define COMPLIE_LR1_DFA_H
#include <string>
#include <map>
#include <unordered_set>
#include "Generators.h"


class LR1_DFA {
    struct LR1_Generator {
        const generator* gen;
        const size_t gen_index;
        std::string outlook="#";
        const int dot=0;
        explicit LR1_Generator(const generator* g, size_t index, const std::string& outlook="#", int dot=0);
        std::string get_outlook() const;
        std::string get_after_dot() const;
        bool operator==(const LR1_Generator& another) const;
        bool operator<(const LR1_Generator& another) const;
        void show() const;
        std::string str() const;
        std::string s;
    };

    struct Node {
        std::set<LR1_DFA::LR1_Generator> generator_list;
        std::map<std::string, Node*> transfer;
        // 等号运算符判断两个节点内部产生式是否完全一样
        bool operator==(const Node& another) const;
        // 计算闭包
        void closure(Generators& generators);
        size_t index = 0;
        std::string hash_key;
        std::string str();
    };

    Node* root = nullptr;
    std::vector<Node*> all_nodes;
    std::map<std::string, std::vector<Node*>> hash_table;
    void add_table(Node* );
    Generators &generators;
    void build();
public:
    std::vector<std::vector<std::pair<char, size_t>>> get_table(std::map<std::string, size_t>& index);

    explicit LR1_DFA(Generators& generators);
    ~LR1_DFA();
    void show();
};


#endif //COMPLIE_LR1_DFA_H
