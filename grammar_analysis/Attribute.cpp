//
// Created by wmj on 18-11-28.
//

#include <stack>
#include "Attribute.h"
#include "../word_analysis/WordAnalysis.h"
#include "../symbol_table/SymbolTable.h"
#include "../symbol_table/Quaternary.h"

using C_GramData = Token;
using namespace std;
void* default_builder(const Token& tk) {
    if (tk.first == TOKEN_ID) {
        return new string(SL.get_identification(tk.second));
    }
    if (tk.first == TOKEN_KEY) {
        string s = SL.get_key(tk.second);
        if (s == "if")
            return attr_builder_if();
        else if (s == "else")
            return attr_builder_else();
    }
    if (tk.first == TOKEN_BOUND) {
        string b = SL.get_bound(tk.second);
        if (b == "{")
            ST.in();
        else if (b == "}")
            ST.leave();
    }
    return new C_GramData( tk );
}


void* attr_builder_if() {
    size_t pos = QL.size();
    quat(OP::IF, 0, 0, 0); // 先用0占位，防止生成临时变量
    return new size_t(pos);
}

void* attr_builder_else() {
    size_t pos = QL.size();
    quat(OP::EL, 0, 0, 0);
    return new size_t(pos);
}

void* attr_endif(std::vector<void*>& v) {
    size_t pos = QL.size();
    quat(OP::EI, 0, 0, 0);
    size_t if_pos = *((size_t*)v[0]);
    QL[if_pos].num1 = ITEM_V(2);
    if (v.size() == 7) {
        // else
        size_t else_pos = *((size_t*)v[5]);
        QL[else_pos].tar = pos;
        QL[if_pos].tar = else_pos;
    } else {
        QL[if_pos].tar = pos;
    }
}