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
        // 生成临时变量的二元运算符
        case OP::BIT_AND:
        case OP::POS:
        case OP::NEG:
        case OP::INV:
        case OP::NOT:
        case OP::GREATER_THEN:
        case OP::GREATER_EQUAL:
        case OP::LESS_EQUAL:
        case OP::LESS_THAN:
        case OP::EQUAL:
        case OP::PLUS:
        case OP::MINUS:
        case OP::MULTPLY:
        case OP::DIVIDE:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST[num1].type, Cat_Temp, 0});
            break;
        case OP::CALL:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST.get_function_type(num1), Cat_Temp, 0});
            break;
        // 不需要生成临时变量, 无目标或后填充
        case OP::INC:
        case OP::DEC:
        case OP::ASSIGN:
        case OP::IF:
        case OP::EL:
        case OP::EI:
        case OP::WH:
        case OP::DO:
        case OP::EW:
        case OP::NOP:
        case OP::FOR:
        case OP::JMP:
        case OP::EFOR:
            break;
        default:
            throw runtime_error( "<Quaternary.cpp> not support " + to_string(static_cast<int>(op)) + " yet");

    }
    QL.emplace_back(op, num1, num2, t);
    return t;
}


string op2str(OP& op) {

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
        os << i << "# ";
        switch (t.op) {
            case OP::PLUS:os << "(+, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::MINUS:os << "(-, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::MULTPLY:os << "(*, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::DIVIDE:os << "(/, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::MOD:os << "(%, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::BIT_AND:os << "(&, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::BIT_OR:os << "(|, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::BIT_XOR:os << "(^, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::SHIFT_LEFT:os << "(<<, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::SHIFT_RIGHT:os << "(>>, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::LESS_THAN:os << "(<, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::GREATER_THEN:os << "(>, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::LESS_EQUAL:os << "(<=, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::GREATER_EQUAL:os << "(>=, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::EQUAL:os << "(==, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::NOT_EQUAL:os << "(!=, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::LOG_AND:os << "(&&, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::LOG_OR:os << "(||, "<< ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl;break;
            case OP::INC:os << "(++, " << ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::DEC:os << "(--, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::ASSIGN:os << "(=, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::GET_ADDR:os << "(&, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::GET_VALUE:os << "(*, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::CALL:os << "(call, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::RET:os << "(ret, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::PUSH:os << "(push, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::POP:os << "(pop, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::NEG:os << "(-, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::POS:os << "(+, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::NOT:os << "(!, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::INV:os << "(~, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::IF:os << "(if, "<< ST[t.num1] << ", _ , " << t.tar << ")" << endl; break;
            case OP::FUNC:os << "(func, " << endl; break;
            case OP::EFUNC:os << "(end-func, " << endl; break;
            case OP::EL:os << "(else, " << ", _ ," << t.tar << ")" << endl; break;
            case OP::WH:os << "(while, _, _, _)" << endl; break;
            case OP::FOR:os << "(for, _, _, _)" << endl; break;
            case OP::DO:os << "(do, " << ST[t.num1] << ", _, " << t.tar <<")" << endl; break;
            case OP::EW:os << "(end-while, _, _, " << t.tar << ")" << endl; break;
            case OP::EFOR:os << "(end-for, _, _, " << t.tar << ")" << endl; break;
            case OP::JMP:os << "(jmp, _, _, " << t.tar << ")" << endl; break;
            case OP::EI:os << "(end-if, _, _, _)" << endl; break;
        }
    }
    return os;
}
