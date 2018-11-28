//
// Created by wmj on 18-11-28.
//

#ifndef COMPLIE_ATTR_EXPQUAT_H
#define COMPLIE_ATTR_EXPQUAT_H

#include "Attribute.h"
#include <stack>
#include <vector>
#include <string>
#include <iostream>
#include "../../word_analysis/TokenList.h"

struct Quaternary {
    std::string op;
    std::string num1;
    std::string num2;
    std::string target;
};

std::ostream& operator<<(std::ostream& out, const Quaternary& quat);

class Attr_ExpQuat_2_LR : public Attribute {
    static int t_cnt;
    TokenList& tokenList;
public:
    explicit Attr_ExpQuat_2_LR(TokenList& tokenList);
    void *operator()(const std::vector<void*>& sub_tokens) override;
    void *get_data(TokenList::iterator& it) override;
};

class process_B: public Attribute {
public:
    void *operator()(const std::vector<void*>& sub_tokens) override;
    void *get_data(TokenList::iterator&it) override {return nullptr;}
};


#endif //COMPLIE_ATTR_EXPQUAT_H
