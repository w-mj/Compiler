//
// Created by wmj on 18-11-15.
//

#include "LR1_DFA.h"
#include <queue>
#include <utility>

using namespace std;
void LR1_DFA::build(Generators &generators, std::string start) {
    root = new Node();
    all_nodes.push_back(root);
    root->generator.emplace_back(make_generator("@Start", start));

    queue<LR1_Generator> to_extend;  // 待扩展生成式
    to_extend.push(root->generator[0]);
    
    while (!to_extend.empty()) {
        LR1_Generator expend = to_extend.front();
        to_extend.pop();

        const string& after_dot = expend.get_after_dot();  // 点后面的第一个符号
        if (generators.isVN(after_dot)) {
            // 扩展点后面的第一个非终结符
            vector<generator> extended_generators = generators.get_generators(after_dot);  // 找到点后非终结符的所有产生式
            for (const generator &gen: extended_generators)
                LR1_Generator* new_generator = new LR1_Generator(gen);
        }

    }
}


std::string LR1_DFA::LR1_Generator::get_after_dot() const {
    if (dot < gen.second.size() - 1)
        return gen.second[dot];
    else
        return "#";
}

std::string LR1_DFA::LR1_Generator::get_suffix() const {
    if (dot < gen.second.size() - 2)
        return gen.second[dot + 1];
    else
        return "#";
}

LR1_DFA::LR1_Generator::LR1_Generator(const generator& g, const string &suffix, int dot)
        : gen(g), suffix(suffix), dot(dot) {

}
