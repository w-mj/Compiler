//
// Created by wmj on 18-12-5.
//

#include "Quaternary.h"
#include "../word_analysis/WordAnalysis.h"
#include <iostream>
#include <algorithm>
using namespace std;

#define pq(a, b, c, d) (cout << "{"##a##", " << (b) << ", " << (c) << ", " << (d) << "}")

void make(OP op, size_t num1, size_t num2, size_t target) {}

size_t quat(OP op, size_t num1, size_t num2) {
    size_t t = num1;
    switch (op) {
        case OP::PLUS:
            t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
            pq("+", ST[num1].name, ST[num2].name, ST[t].name);
            break;
        case OP::CALL:
            t = ST.add_symbol({ST.get_temp_var_name(), ST.get_function_type(num1), Cat_Temp, 0});
            pq("call", ST[num1].name, "_", ST[t].name);
            break;
        case OP::INC:
            pq("++", ST[num1].name, "_", "_");
            break;
        case OP::DEC:
            pq("--", ST[num1].name, "_", "_");
            break;
        default:
            throw runtime_error( "not support " + to_string(static_cast<int>(op)) + "yet");
    }
    return t;

}

size_t make_unary_operator_quat(void* it, size_t num1) {
    auto op = ((Item*)it) -> second;
    size_t t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
    switch (WordAnalysis::bound[op].c_str()[0]) {
        case '&':
            pq("&", ST[num1].name, "_", ST[t].name);
            return t;
        case '*':
            pq("*", ST[num1].name, "_", ST[t].name);
            return t;
        case '+':
            pq("+", ST[num1].name, "_", ST[t].name);
            return t;
        case '-':
            pq("-", ST[num1].name, "_", ST[t].name);
            return t;
        case '~':
            pq("~", ST[num1].name, "_", ST[t].name);
            return t;
        case '!':
            pq("!", ST[num1].name, "_", ST[t].name);
            return t;
        default:
            throw runtime_error("wrong in make_unary_operator_quat");
    }
}

size_t make_assign_operator_quat(size_t l, size_t a, size_t r) {
    static const long pl = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), "+=") - WordAnalysis::bound.begin();
    static const long mi = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), "-=") - WordAnalysis::bound.begin();
    static const long ml = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), "*=") - WordAnalysis::bound.begin();
    static const long di = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), "/=") - WordAnalysis::bound.begin();
    static const long mo = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), "%=") - WordAnalysis::bound.begin();
    static const long lf = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), "<<=") - WordAnalysis::bound.begin();
    static const long lr = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), ">>=") - WordAnalysis::bound.begin();
    static const long bo = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), "|=") - WordAnalysis::bound.begin();
    static const long ba = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), "&=") - WordAnalysis::bound.begin();
    static const long bx = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), "^=") - WordAnalysis::bound.begin();
    static const long as = find(WordAnalysis::bound.begin(), WordAnalysis::bound.end(), "=") - WordAnalysis::bound.begin();

    switch (a) {
        case pl: return quat(OP::PLUS, l, r, l);
        case mi: return quat(OP::MINUS, l, r, l);
        case ml: return quat(OP::MULTPLY, l, r, l);
        case di: return quat(OP::DIVIDE, l, r, l);
        case mo: return quat(OP::MOD, l, r, l);
        case lf: return quat(OP::SHIFT_LEFT, l, r, l);
        case lr: return quat(OP::SHIFT_RIGHT, l, r, l);
        case bo: return quat(OP::BIT_OR, l, r, l);
        case ba: return quat(OP::BIT_AND, l, r, l);
        case bx: return quat(OP::BIT_XOR, l, r, l);
        case as: return quat(OP::ASSIGN, r, 0, l);
    }

}