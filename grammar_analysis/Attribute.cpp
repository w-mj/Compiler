//
// Created by wmj on 18-11-28.
//

#include "Attribute.h"

struct C_GramData {
    Token d;
};

void* default_builder(const Token& tk) {
    auto data = new C_GramData{ tk };
    return data;
}