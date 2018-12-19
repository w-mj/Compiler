//
// Created by wmj on 18-11-28.
//

#include <stack>
#include "Attribute.h"
#include "../word_analysis/WordAnalysis.h"
#include "../symbol_table/SymbolTable.h"
#include "../symbol_table/Quaternary.h"
#include "../error/Error.h"

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
                // brackets_stack.push("for");
                return nullptr;
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
                        brackets_stack.pop();
                        return attr_builder_if();
                    }
//                    else if (brackets_stack.top() == "for2") {
//                        brackets_stack.pop();
//                        return attr_builder_for_inc();
//                    }
                }
                break;
            case psimi:
                break;
//                if (!brackets_stack.empty()) {
//                    if (brackets_stack.top() == "(") {
//                        brackets_stack.pop();
//                        if (!brackets_stack.empty()) {
//                            if (brackets_stack.top() == "for") {
//                                brackets_stack.pop();
//                                brackets_stack.push("for1");
//                                brackets_stack.push("(");
//                                return attr_builder_for_init();
//                            } else if (brackets_stack.top() == "for1") {
//                                brackets_stack.pop();
//                                brackets_stack.push("for2");
//                                brackets_stack.push("(");
//                                return attr_builder_for_cond();
//                            }
//                        }
//                    }
//                }
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
    // return new size_t(if_pos);
    if (v.size() == 7) {
        auto x = static_cast<vector<size_t*>*>(v[4]);
        auto y = static_cast<vector<size_t*>*>(v[6]);
        x->insert(x->begin(), y->begin(), y->end());
        delete y;
        return x;
    }
    return v[4];
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
    auto * jmp_quat = static_cast<vector<size_t*> *>(v[4]);
    for (auto x: *jmp_quat) {
        if (x != nullptr) {
            size_t t = *x;
            if (QL[t].op == OP::BREAK)
                QL[t].tar = QL.size();
            else if (QL[t].op == OP::CONTINUE)
                QL[t].tar = while_pos;
        }
    }
    return nullptr;
}

void* attr_stmt_pos() {
//    static size_t stmt_start = 0;
//    size_t a = stmt_start;
//    cout << a << endl;
//    stmt_start = QL.size();
//    return new size_t(a);
    return nullptr;
}


void* attr_builder_for_init() {
    size_t t = QL.size();
    quat(OP::FOR, 0, 0, 0);
    return new size_t(t);
}

void* attr_builder_for_cond(size_t cond) {
    size_t t = QL.size();
    quat(OP::DO, cond, 0, 0);
    quat(OP::JMP, 0, 0, 0);
    return new size_t(t);
}

void* attr_builder_for_inc() {
    size_t t = QL.size();
    quat(OP::JMP, 0, 0, 0);
    return new size_t(t);
}

void* attr_endfor(std::vector<void*>& v) {
    size_t for_pos = *((size_t*)v[2]);
    size_t do_pos = *((size_t*)v[3]);
    size_t jmp_pos = v.size() == 6? *((size_t*)v[4]): *((size_t*)v[5]);
    quat(OP::EFOR, 0, 0, do_pos + 2);
//    if (do_pos == for_pos + 1)
//        QL[do_pos].num1 = ST.get_symbol_index_by_name("@const_1");
//    else
//        QL[do_pos].num1 = QL[do_pos - 1].tar;
    QL[do_pos].tar = QL.size();
    QL[jmp_pos].tar = for_pos;
    QL[do_pos + 1].tar = jmp_pos + 1;
    return nullptr;
}

void* attr_start_func(std::vector<void*>& v) {
    size_t quat_pos = SymbolTable::TempSymbol::add_basic_type_and_insert_into_table(
            (SymbolTable::TempSymbol*)v[1], (SymbolTable::Type*)v[0], Cat_Func_Defination);

    auto temp = (SymbolTable::TempSymbol*)v[1];
    map<string, size_t> index;
    for (size_t i = 0; i < temp->fpl.size(); i++) {
        index.emplace(temp->fpl[i]->s.name, ST.get_function_by_index(ST.get_type_by_index(temp->s.type).data).param_index + i);
    }
    ST.in(index);
    return nullptr;
}

void* attr_end_func(std::vector<void*>& v) {
    quat(OP::EFUNC, 0, 0, 0);
    return nullptr;
}