//
// Created by wmj on 18-12-5.
//

#include "Quaternary.h"
#include "../word_analysis/WordAnalysis.h"
#include <iostream>
#include <fstream>
#include <algorithm>
#include "../Utility.h"
#include "../error/Error.h"

#define POS1(x) ST[x].line, ST[x].s, ST[x].e

using namespace std;

size_t type_uplift(size_t t1, size_t t2) {
    int t1t = ST.get_type_by_index(t1).t;
    int t2t = ST.get_type_by_index(t2).t;
    if (t1t > t2t)
        return t1;
    else
        return t2;
}

bool is_basic_type(size_t t) {
    return oneof(ST.get_type_by_index(t).t, SHORT, INT, LONG, FLOAT, DOUBLE, CHAR);
}

bool is_integer_type(size_t t) {
    return oneof(ST.get_type_by_index(t).t, SHORT, INT, LONG);
}

size_t quat(OP op, size_t num1, size_t num2, size_t t) {
    switch (op) {
        // 生成临时变量的二元运算符
        case OP::GREATER_THEN:
        case OP::GREATER_EQUAL:
        case OP::LESS_EQUAL:
        case OP::LESS_THAN:
        case OP::EQUAL:
            if (!is_basic_type(ST[num1].type) || !ST[num2].type)
                error("ERROR: Invalid operands to binary " + op_to_str(op) + " (" +
                      ST.get_top_type_name(num1) + " and " + ST.get_top_type_name(num2) + ")");
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), type_uplift(ST[num1].type, ST[num2].type), Cat_Temp, 0});
            break;
        case OP::BIT_AND:
        case OP::PLUS:
        case OP::MINUS:
        case OP::MULTPLY:
        case OP::DIVIDE:
        case OP::LOG_OR:
        case OP::LOG_AND:
        case OP::BIT_XOR:
        case OP::BIT_OR:
            if (!is_basic_type(ST[num1].type) || !ST[num2].type)
                error("ERROR: Invalid operands to binary " + op_to_str(op) + " (" +
                 ST.get_top_type_name(num1) + " and " + ST.get_top_type_name(num2) + ")");
            if (ST[num1].type != ST[num2].type)
                warring("WARRING: Different type on operands to binary " + op_to_str(op) + " (" +
                 ST.get_top_type_name(num1) + " and " + ST.get_top_type_name(num2) + ")");
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), type_uplift(ST[num1].type, ST[num2].type), Cat_Temp, 0});
            break;
        case OP::INDEX:
            if (!is_integer_type(ST[num2].type))
                error("Array index must be integer. (" + ST.get_top_type_name(num2) + ")");
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST.get_basic_symbol_type(num1), Cat_Temp, 0});
            ST[t].is_addr = true;
            break;
        case OP::MEMBER:
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), 0, Cat_Temp, 0});
            ST[t].is_addr = true;
        case OP::PMEMBER:
            break;
        case OP::CALL:
            if (ST.get_type_by_symbol(num1).t != FUNCTION)
                error(ST[num1].name + " is not a function.");
            if (!t) t = ST.add_symbol({ST.get_temp_var_name(), ST.get_function_type(num1), Cat_Temp, 0});
            break;

        // 不需要生成临时变量, 无目标或后填充

        case OP::ASSIGN:
        case OP::INC:
        case OP::DEC:
            if (!is_basic_type(ST[num1].type) || !is_basic_type(ST[t].type))
                error("ERROR: Invalid operands to = (" + ST.get_top_type_name(num1) + " and " + ST.get_top_type_name(t) + ")");
        case OP::POS:
        case OP::NEG:
        case OP::INV:
        case OP::NOT:
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
        case OP::RETN:
        case OP::GOTO:
        case OP::LABEL:
        case OP::CONTINUE:
        case OP::BREAK:
        case OP::PUSH:
        case OP::ASM:
        case OP::EDEF_STRU:
        case OP::DEF_STRU_ELE:
        case OP::DEF_STRU:
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
    ofstream off;
    off.open("siyuan.txt");
    for (int i = 0; i < ql.size(); i++) {
        off << static_cast<int>(ql[i].op) << " " << ql[i].num1 << " " << ql[i].num2 << " " <<  ql[i].tar << endl;
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
            case OP::EFUNC:os << "(end-func, " << ST[t.num1] << ", _, _)" << endl; break;
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
            case OP::DEF_FUN:os << "(def-fun, " << ST[t.num1] << ", _, _)" << endl; break;
            case OP::DEF_STRU_ELE:os << "(def-struct-ele, " << ST[t.num1] << ", _, _)" << endl; break;
            case OP::DEF_STRU:os << "(def-struct, " << ST[t.num1] << ", _, _)" << endl; break;
            case OP::EDEF_STRU:os << "(end-def-struct, _, _, _)" << endl; break;
            case OP::DEF_VAR:os << "(def-var, " << ST[t.num1] << ", _, _)" << endl; break;
            case OP::GOTO: os << "(goto, _, _, " << ST[t.tar] << ")" << endl; break;
            case OP::LABEL: os << "(label, " << ST[t.num1] << ", _, _)" << endl; break;
            case OP::RETN: os << "(retn, " << ST[t.num1] << ", _, _)" << endl; break;
            case OP::ASM: os << "(ASM, " << ST.get_asm(t.num1) << " , _, _)" << endl; break;
            case OP::INDEX: os << "([], " << ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl; break;
            case OP::MEMBER: os << "(., " << ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl; break;
            case OP::PMEMBER: os << "(->, " << ST[t.num1] << ", " << ST[t.num2] << ", " << ST[t.tar] << ")" << endl; break;

            case OP::NOP: os << "(nop, _, _, _)" << endl; break;
            default:
                os << debugpos << "unsupport quat" << endl;
        }
    }
    off.close();
    return os;
}

std::vector<Quat> &QuatList::get_vector() {
    return quat_list;
}

void QuatList::clear() {
    quat_list.clear();
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
        case OP::INDEX:return "[]";
        case OP::RETN:return "retn";
        case OP::ASM:return "ASM";
        case OP::DEF_STRU:return "def-stru";
        case OP::DEF_STRU_ELE:return "def-stru_element";
        case OP::EDEF_STRU:return "end-def-stru";
        case OP::MEMBER:return ".";
        case OP::PMEMBER:return "->";
    }
    throw runtime_error(debugpos + " unknown op");
}


size_t make_struct_member_quat(size_t str, std::string* member) {
    if (ST.get_type_by_symbol(str).t != STRUCT)
        error("member operator (.) must operated on struct. (" + ST.get_top_type_name(str) + ")");
    auto& stru = ST.get_struct_by_symbol(str);
    for (size_t i = 0; i < stru.num_fields; i++) {
        if (ST[stru.data + i].name == *member) {
            Number c{};
            c.type = Number::Int;
            c.value.si = ST[stru.data + i].offset;
            size_t temp_var = quat(OP::MEMBER, str, ST.get_or_add_constant(c));
            ST[temp_var].type = ST[stru.data + i].type;
            return temp_var;
        }
    }
    error("struct " + ST[str].name + " doesn't have member " + *member);
    return 0;
}