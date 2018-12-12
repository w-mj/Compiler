//
// Created by wmj on 18-12-5.
//

#include "Quaternary.h"
#include "../word_analysis/WordAnalysis.h"
#include <iostream>
#include <algorithm>
using namespace std;

#define pq(a, b, c, d) (cout << quat_cnt << "{" << (a) << ", " << (b) << ", " << (c) << ", " << (d) << "}" << endl)

size_t quat(OP op, size_t num1, size_t num2, size_t t) {
    static int quat_cnt = 0;
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
            pq("=", ST[num1], "_", ST[t]);
            break;
        case OP::BIT_AND:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
            pq("&", ST[num1], "_", ST[t]);
            break;
        case OP::POS:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
            pq("+", ST[num1], "_", ST[t]);
            break;
        case OP::NEG:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
            pq("-", ST[num1], "_", ST[t]);
            break;
        case OP::INV:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
            pq("~", ST[num1], "_", ST[t]);
            break;
        case OP::NOT:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
            pq("!", ST[num1], "_", ST[t]);
            break;
        default:
            throw runtime_error( "not support " + to_string(static_cast<int>(op)) + " yet");
    }
    quat_cnt++;
    return t;
}

size_t make_unary_operator_quat(void* it, size_t num1) {
    auto op = ((Item*)it) -> second;
    size_t t;
    switch (WA.bound[op].c_str()[0]) {
        case '&':
            t = quat(OP::BIT_AND, num1, 0);
            return t;
        case '*':
            t = quat(OP::MULTPLY, num1, 0);
            return t;
        case '+':
            t = quat(OP::POS, num1, 0);
            return t;
        case '-':
            t = quat(OP::NEG, num1, 0);
            return t;
        case '~':
            t = quat(OP::INV, num1, 0);
            return t;
        case '!':
            t = quat(OP::NOT, num1, 0);
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