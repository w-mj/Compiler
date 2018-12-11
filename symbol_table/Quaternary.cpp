//
// Created by wmj on 18-12-5.
//

#include "Quaternary.h"
#include "../word_analysis/WordAnalysis.h"
#include <iostream>
#include <algorithm>
using namespace std;

#define pq(a, b, c, d) (cout << "{" << a << ", " << (b) << ", " << (c) << ", " << (d) << "}" << endl)

void make(OP op, size_t num1, size_t num2, size_t target) {}

size_t quat(OP op, size_t num1, size_t num2, size_t t) {
    switch (op) {
        case OP::PLUS:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
            pq("+", ST[num1], ST[num2], ST[t]);
            break;
        case OP::CALL:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST.get_function_type(num1), Cat_Temp, 0});
            pq("call", ST[num1], "_", ST[t]);
            break;
        case OP::INC:
            pq("++", ST[num1], "_", "_");
            break;
        case OP::DEC:
            pq("--", ST[num1], "_", "_");
            break;
        case OP::ASSIGN:
            pq("=", ST[num1], "_", ST[num2]);
            break;
        default:
            throw runtime_error( "not support " + to_string(static_cast<int>(op)) + " yet");
    }
    return t;

}

size_t make_unary_operator_quat(void* it, size_t num1) {
    auto op = ((Item*)it) -> second;
    size_t t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
    switch (WA.bound[op].c_str()[0]) {
        case '&':
            pq("&", ST[num1], "_", ST[t]);
            return t;
        case '*':
            pq("*", ST[num1], "_", ST[t]);
            return t;
        case '+':
            pq("+", ST[num1], "_", ST[t]);
            return t;
        case '-':
            pq("-", ST[num1], "_", ST[t]);
            return t;
        case '~':
            pq("~", ST[num1], "_", ST[t]);
            return t;
        case '!':
            pq("!", ST[num1], "_", ST[t]);
            return t;
        default:
            throw runtime_error("wrong in make_unary_operator_quat");
    }
}

size_t make_assign_operator_quat(size_t l, size_t a, size_t r) {
    string oper = WA.bound[a];
    size_t t;
    if (oper == "=")
        t = quat(OP::ASSIGN, r, 0, l);
    else if (oper == "+=")
        t = quat(OP::PLUS, l, r, l);
    else if (oper == "-=")
        t = quat(OP::MINUS, l, r, l);
    else if (oper == "*=")
        t = quat(OP::MULTPLY, l, r, l);
    else if (oper == "/=")
        t = quat(OP::DIVIDE, l, r, l);
    else if (oper == "%=")
        t = quat(OP::MOD, l, r, l);
    else if (oper == "<<=")
        t = quat(OP::SHIFT_LEFT, l, r, l);
    else if (oper == ">>=")
        t = quat(OP::SHIFT_RIGHT, l, r, l);
    else if (oper == "|=")
        t = quat(OP::BIT_OR, l, r, l);
    else if (oper == "&=")
        t = quat(OP::BIT_AND, l, r, l);
    else if (oper == "^=")
        t = quat(OP::BIT_XOR, l, r, l);
    return t;
}