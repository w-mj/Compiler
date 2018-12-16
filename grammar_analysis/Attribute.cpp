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
    static stack <string> brackets_stack;

    if (tk.first == TOKEN_ID) {
        return new string(SL.get_identification(tk.second));
    }
    if (tk.first == TOKEN_KEY) {
        switch(tk.second) {
            case kif:
                brackets_stack.push("if");
                return nullptr;
            case kelse:
                return attr_builder_else();
            case kwhile:
                brackets_stack.push("while");
                return attr_builder_while();
            case kfor:
                brackets_stack.push("for");
                return attr_builder_for();
        }
    }
    if (tk.first == TOKEN_BOUND) {
        switch (tk.second) {
            case plc:
                ST.in();
                return nullptr;
            case prc:
                ST.leave();
                return nullptr;
            case plp:
                brackets_stack.push("(");
                return nullptr;
            case prp:
                brackets_stack.pop();
                if (!brackets_stack.empty()) {
                    if (brackets_stack.top() == "while") {
                        brackets_stack.pop();
                        return attr_builder_while_do();
                    } else if (brackets_stack.top() == "if") {
                        brackets_stack.top();
                        return attr_builder_if();
                    }
                }
        }
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
    size_t if_pos = *((size_t*)v[3]);
    QL[if_pos].num1 = ITEM_V(2);
    if (v.size() == 7) {
        // else
        size_t else_pos = *((size_t*)v[5]);
        QL[else_pos].tar = pos;
        QL[if_pos].tar = else_pos;
    } else {
        QL[if_pos].tar = pos;
    }
    attr_stmt_pos();
    return new size_t(if_pos);
}

void* attr_builder_while() {
    size_t pos = QL.size();
    quat(OP::WH, 0, 0, 0);
    attr_stmt_pos();
    return new size_t(pos);
}

void* attr_builder_while_do() {
    size_t pos = QL.size();
    quat(OP::DO, 0, 0, 0);
    attr_stmt_pos();
    return new size_t(pos);
}

void* attr_endwhile(std::vector<void*>& v) {
    size_t while_pos = *((size_t*)v[0]);
    size_t do_pos = *((size_t*)v[3]);
    quat(OP::EW, 0, 0, while_pos);
    QL[do_pos].num1 = QL[do_pos - 1].tar;
    QL[do_pos].tar = QL.size();
    return new size_t(while_pos);
}

void* attr_stmt_pos() {
//    static size_t stmt_start = 0;
//    size_t a = stmt_start;
//    cout << a << endl;
//    stmt_start = QL.size();
//    return new size_t(a);
    return nullptr;
}


void* attr_builder_for() {
    size_t t = QL.size();
    quat(OP::FOR, 0, 0, 0);
    return new size_t(t);
}