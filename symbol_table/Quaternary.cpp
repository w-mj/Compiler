//
// Created by wmj on 18-12-5.
//

#include "Quaternary.h"
#include "../word_analysis/WordAnalysis.h"
#include <iostream>
#include <algorithm>
#include "../Utility.h"
#include "../error/Error.h"

using namespace std;

size_t type_uplift(size_t t1, size_t t2) {
    return (size_t) max(ST.get_type_by_index(t1).t, ST.get_type_by_index(t2).t);
}

bool is_basic_type(size_t t) {
    return oneof(ST.get_type_by_index(t), SHORT, INT, LONG, FLOAT, DOUBLE);
}

bool is_integer_type(size_t t) {
    return oneof(ST.get_type_by_index(t), SHORT, INT, LONG);

}

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
            if (!is_basic_type(ST[num1].type) || !ST[num2].type)
                error("ERROR: Invalid operands to binary " + op_to_str(op) + " (" +
                 ST.get_top_type_name(num1) + " and " + ST.get_top_type_name(num2) + ")");
        case OP::INDEX:
            if (!is_integer_type(ST[num2].type))
                error("Array index must be integer. (" + ST.get_top_type_name(num2) + ")");
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), type_uplift(ST[num1].type, ST[num2].type), Cat_Temp, 0});
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
        case OP::DEF_FUN:
        case OP::DEF_VAR:
        case OP::FUNC:
        case OP::EFUNC:
        case OP::RET:
        case OP::GOTO:
        case OP::LABEL:
        case OP::CONTINUE:
        case OP::BREAK:
        case OP::PUSH:
            break;
        default:
            throw runtime_error(debugpos+ " not support " + to_string(static_cast<int>(op)) + " yet");

    }
    QL.emplace_back(op, num1, num2, t);
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
            case OP::PUSH:os << "(push, "<< ST[t.num1] << ", _ , _)" << endl; break;
            case OP::POP:os << "(pop, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::NEG:os << "(-, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::POS:os << "(+, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::NOT:os << "(!, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::INV:os << "(~, "<< ST[t.num1] << ", _ , " << ST[t.tar] << ")" << endl; break;
            case OP::IF:os << "(if, "<< ST[t.num1] << ", _ , " << t.tar << ")" << endl; break;
            case OP::FUNC:os << "(func, " << ST[t.num1] << ", _, _)" << endl; break;
            case OP::EFUNC:os << "(end-func, _, _, _)" << endl; break;
            case OP::EL:os << "(else, " << ", _ ," << t.tar << ")" << endl; break;
            case OP::WH:os << "(while, _, _, _)" << endl; break;
            case OP::FOR:os << "(for, _, _, _)" << endl; break;
            case OP::DO:os << "(do, " << ST[t.num1] << ", _, " << t.tar <<")" << endl; break;
            case OP::EW:os << "(end-while, _, _, " << t.tar << ")" << endl; break;
            case OP::EFOR:os << "(end-for, _, _, " << t.tar << ")" << endl; break;
            case OP::JMP:os << "(jmp, _, _, " << t.tar << ")" << endl; break;
            case OP::CONTINUE:os << "(continue, _, _, " << t.tar << ")" << endl; break;
            case OP::BREAK:os << "(break, _, _, " << t.tar << ")" << endl; break;
            case OP::EI:os << "(end-if, _, _, _)" << endl; break;
            case OP::DEF_FUN:os << "(def_fun, " << ST[t.num1] << ", _, _)" << endl; break;
            case OP::DEF_VAR:os << "(def_var, " << ST[t.num1] << ", _, _)" << endl; break;
            case OP::GOTO: os << "(goto, _, _, " << ST[t.tar] << ")" << endl; break;
            case OP::LABEL: os << "(label, " << ST[t.num1] << ", _, _)" << endl; break;
            case OP::INDEX: os << "([], " << ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl; break;

            default:
                os << debugpos << "unsupport quat" << endl;
        }
    }
    return os;
}

std::string op_to_str(OP op) {
    switch (op) {
        case OP::PUSH: return "push";
        case OP::BREAK: return "break";
        case OP::PLUS: return "+";
        case OP::MINUS:return "-";
        case OP::MULTPLY:return "*";
        case OP::DIVIDE:return "/";
        case OP::MOD:return "%";
        case OP::INC:return "++";
        case OP::DEC:return "--";
        case OP::BIT_AND: return "&";
        case OP::BIT_OR: return "|";
        case OP::BIT_XOR: return "^";
        case OP::SHIFT_LEFT:return"<<";
        case OP::SHIFT_RIGHT: return">>";
        case OP::ASSIGN:return"=";
        case OP::GET_ADDR:return"&";
        case OP::GET_VALUE:return"*";
        case OP::CALL:return"call";
        case OP::RET:return"return";
        case OP::POP:return"pop";
        case OP::NEG:return"-";
        case OP::POS:return"+";
        case OP::NOT:return"!";
        case OP::INV:return"~";
        case OP::LESS_THAN:return"<";
        case OP::GREATER_THEN:return">";
        case OP::LESS_EQUAL:return"<=";
        case OP::GREATER_EQUAL:return">=";
        case OP::EQUAL:return"==";
        case OP::NOT_EQUAL:return"!=";
        case OP::LOG_AND:return"&&";
        case OP::LOG_OR:return"||";
        case OP::FUNC:return"func";
        case OP::EFUNC:return"end-func";
        case OP::IF:return"if";
        case OP::EI:return"else";
        case OP::EL:return"end-if";
        case OP::WH:return"while";
        case OP::DO:return"do";
        case OP::EW:return"end-while";
        case OP::FOR:return"for";
        case OP::JIF:return"jz";
        case OP::EFOR:return"end-for";
        case OP::NOP:return"nop";
        case OP::JMP:return"jmp";
        case OP::DEF_FUN:return"def";
        case OP::DEF_VAR:return"var";
        case OP::LABEL:return"LABEL";
        case OP::GOTO:return"goto";
        case OP::CONTINUE:return"continue";
    }
    throw runtime_error(debugpos + " unknown op");
}
