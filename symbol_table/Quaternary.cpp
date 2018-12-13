//
// Created by wmj on 18-12-5.
//

#include "Quaternary.h"
#include "../word_analysis/WordAnalysis.h"
#include <iostream>
#include <algorithm>

using namespace std;

size_t quat(OP op, size_t num1, size_t num2, size_t t) {
    switch (op) {
        case OP::PLUS:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
            break;
        case OP::CALL:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST.get_function_type(num1), Cat_Temp, 0});
            break;
        case OP::INC:
            break;
        case OP::DEC:
            break;
        case OP::ASSIGN:
            break;
        case OP::BIT_AND:
        case OP::POS:
        case OP::NEG:
        case OP::INV:
        case OP::NOT:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
            break;
        case OP::IF:
        case OP::EL:
        case OP::EI:
            break;
        default:
            throw runtime_error( "not support " + to_string(static_cast<int>(op)) + " yet");

    }
    QL.emplace_back(op, num1, num2, t);
    return t;
}


string op2str(OP& op) {
    switch (op) {
        case OP::PLUS:return "+";
        case OP::MINUS:return "-";
        case OP::MULTPLY:return "*";
        case OP::DIVIDE:return "/";
        case OP::MOD:return "%";
        case OP::INC:return "++";
        case OP::DEC:return "--";
        case OP::BIT_AND:return "&";
        case OP::BIT_OR:return "|";
        case OP::BIT_XOR:return "^";
        case OP::SHIFT_LEFT:return "<<";
        case OP::SHIFT_RIGHT:return ">>";
        case OP::ASSIGN:return "=";
        case OP::GET_ADDR:return "&";
        case OP::GET_VALUE:return "*";
        case OP::CALL:return "call";
        case OP::RET:return "ret";
        case OP::PUSH:return "push";
        case OP::POP:return "pop";
        case OP::NEG:return "-";
        case OP::POS:return "+";
        case OP::NOT:return "!";
        case OP::INV:return "~";
        case OP::LESS_THAN:return "<";
        case OP::GREATER_THEN:return ">";
        case OP::LESS_EQUAL:return "<=";
        case OP::GREATER_EQUAL:return ">=";
        case OP::EQUAL:return "==";
        case OP::NOT_EQUAL:return "!=";
        case OP::LOG_AND:return "&&";
        case OP::LOG_OR:return "||";
        case OP::FUNC:return "func";
        case OP::EFUNC:return "end-func";
        case OP::IF:return "if";
        case OP::EI:return "end-if";
        case OP::EL:return "else";
        case OP::WH:return "while";
        case OP::DO:return "do";
        case OP::EW:return "end-while";
    }
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

Quat &QuatList::operator[](size_t i) {
    return quat_list[i];
}

size_t QuatList::size() {
    return quat_list.size();
}

std::ostream& operator<<(std::ostream& os, QuatList& ql) {
    for (int i = 0; i < ql.size(); i++) {
        auto& t = ql[i];
        switch (t.op) {
            case OP::IF:
            case OP::EL:
            case OP::EI:
                (cout << i << "{" << op2str(t.op) << ", " << ST[t.num1] << ", " << ST[t.num2] << ", " << t.tar << "}" << endl);
                break;
            default:
                (cout << i << "{" << op2str(t.op) << ", " << ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << "}" << endl);
        }
    }
}
