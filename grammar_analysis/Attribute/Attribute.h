//
// Created by wmj on 18-11-28.
//

#ifndef COMPLIE_ARRTIBUTE_H
#define COMPLIE_ARRTIBUTE_H

#include <vector>
#include "../../word_analysis/TokenList.h"

class Attribute {
public:
    virtual void* operator()(const std::vector<void*>&) = 0;
    virtual void* get_data(TokenList::iterator &) = 0;
    bool nothing = false;
};

struct Attr_Nothing: public Attribute {
    Attr_Nothing();
    void* operator()(const std::vector<void*>& v) override {return v[0];}
    void* get_data(TokenList::iterator &) override {return nullptr;}
};


#endif //COMPLIE_ARRTIBUTE_H
