//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_LR1_DFA_H
#define COMPLIE_LR1_DFA_H
#include <string>
#include <map>
#include "Generators.h"


class LR1_DFA {
    struct LR1_Generator {
        generator gen;
        std::string outlook="#";
        int dot=0;
        int gen_index;
        explicit LR1_Generator(const generator& g, const std::string& outlook="#", int dot=0);
        std::string get_outlook() const;
        std::string get_after_dot() const;
        bool operator==(const LR1_Generator& another) const;
        void show() const;
    };

    struct Node {
        std::vector<LR1_DFA::LR1_Generator> generator_list;
        std::map<std::string, Node*> transfer;
        // 等号运算符判断只有两个节点内部产生式是否完全一样
        bool operator==(const Node& another) const;
        // equal方法只要两个节点的产生式集合为互相包含关系就为真。
        // 即如果一个节点包含另一个节点的所有主产生式，那么将两个节点全部扩展后会得到完全一样的结果。
        bool equal(const Node& another) const;
        size_t index = 0;
    };

    Node* root = nullptr;
    std::vector<Node*> all_nodes;
    Generators generators;
    void build();
public:
    struct TableStatus {
        char status = 'e';
        union {
            size_t jump;
            generator gen;
        } data;
        TableStatus(char s, generator g);
        TableStatus(char s, size_t j);
    };
    std::vector<std::map<std::string, TableStatus>> get_table();

    explicit LR1_DFA(Generators& generators);
    ~LR1_DFA();
    void show();
};


#endif //COMPLIE_LR1_DFA_H
