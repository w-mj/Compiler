//
// Created by wmj on 18-12-22.
//

#include "80862.h"
#include "../symbol_table/SymbolTable.h"
#include "../symbol_table/Quaternary.h"
#include "../error/Error.h"
#include <iostream>
#include <string>
#include <map>

#define SIGN(x) (((x)>=0?"+ ":"- ") + to_string(x))
#define BP(x) ("SS:[BP " + SIGN(ST[x].offset) + "]")
#define SI(x) ("SS:[SI " + SIGN(ST[x].offset) + "]")


using namespace std;

void pasm(const string& op, const string &n1="", const string& n2="") {
    cout << op << " " << n1 << " " << n2 << endl;
}

map<size_t, size_t> temp_var_table;
size_t local_var_size;
size_t local_temp_size;
size_t get_or_add_temp_var(size_t n) {
    if (ST[n].cat != Cat_Temp)
        return 0;
    if (temp_var_table.find(n) == temp_var_table.end()) {
        temp_var_table.emplace(n, local_temp_size);
        local_temp_size += ST.get_type_size(n);
        ST[n].offset = static_cast<int>(local_temp_size);
    }
    return temp_var_table[n];
}

void code(const string& s) {
    cout << s << endl;
}

constexpr char A = 'A';
constexpr char B = 'B';
constexpr char C = 'C';
constexpr char D = 'D';

string get_operand(size_t symbol) {
    if (ST[symbol].cat == Cat_Const)
        return ST.get_const(symbol).str();
    if (ST[symbol].is_addr) {
        cout << "MOV SI, " << BP(symbol) << endl;
        return "SS:[SI]";
    }
    return BP(symbol);
}

template<char N>
void ld(size_t t) {
    const string& operand = get_operand(t);
    if (ST.get_type_size(t) == 1) {
        cout << "XOR " << N << "H " << N << "H" << endl;
        cout << "MOV " << N << "L, " << operand <<  endl;
    }
    else
        cout << "MOV " << N << "X, " << operand << endl;

}

template<char N1, char N2>
void ld(size_t t1, size_t t2) {
    ld<N1>(t1);
    ld<N2>(t2);
}

template<char N>
void st(size_t t) {
    const string& operand = get_operand(t);
    if (ST.get_type_size(t) == 1)
        cout << "MOV " << operand <<  ", " << N << "L" << endl;
    else
        cout << "MOV " << operand << ", " << N << "X" << endl;
}


void genop(const string& op, size_t n1, size_t n2, size_t tar) {
    ld<A>(n1);
    const string& operand = get_operand(n2);
    switch (getsize(tar)) {
        case 1:
            cout << op << " AL, " << operand << endl;
            break;
        case 2:
            cout << op << " AX, " << operand << endl;
            break;
        default:
            rterr("wrong size " + to_string(getsize(tar)));
    }
    st<A>(tar);

}

void genop(const string& op, size_t n1, size_t tar) {
    ld<A>(n1);
    switch (getsize(tar)) {
        case 1:
            cout << op << " AL" << endl;
            break;
        case 2:
            cout << op << " AX" << endl;
            break;
        default:
            rterr("wrong size " + to_string(getsize(tar)));
    }
    st<A>(tar);
}

int num_operands(const Quat& q) {
    switch (q.op) {
        case OP::PLUS:
        case OP::MINUS:
        case OP::MULTPLY:
        case OP::DIVIDE:
        case OP::MOD:
        case OP::BIT_AND:
        case OP::BIT_OR:
        case OP::BIT_XOR:
        case OP::SHIFT_LEFT:
        case OP::SHIFT_RIGHT:
        case OP::LESS_THAN:
        case OP::GREATER_THEN:
        case OP::LESS_EQUAL:
        case OP::GREATER_EQUAL:
        case OP::EQUAL:
        case OP::NOT_EQUAL:
        case OP::LOG_AND:
        case OP::LOG_OR:
        case OP::INDEX:
        case OP::MEMBER:
        case OP::PMEMBER:
            return 3;

        case OP::INC:
        case OP::DEC:
        case OP::ASSIGN:
        case OP::GET_ADDR:
        case OP::GET_VALUE:
        case OP::CALL:
        case OP::NEG:
        case OP::POS:
        case OP::NOT:
        case OP::INV:
            return 2;

        case OP::PUSH:
        case OP::POP:
        case OP::FUNC:
        case OP::EFUNC:
        case OP::IF:
        case OP::DO:
        case OP::JIF:
        case OP::DEF_FUN:
        case OP::DEF_VAR:
        case OP::LABEL:
        case OP::DEF_STRU:
        case OP::DEF_STRU_ELE:
        case OP::RETN:
            return 1;
        case OP::GOTO:
        case OP::RET:
        case OP::EI:
        case OP::EL:
        case OP::WH:
        case OP::EW:
        case OP::FOR:
        case OP::EFOR:
        case OP::NOP:
        case OP::JMP:
        case OP::CONTINUE:
        case OP::BREAK:
        case OP::EDEF_STRU:
        case OP::ASM:
            return 0;
    }
}


void make8086() {
    for (const Quat& q: QL.get_vector()) {
        switch (num_operands(q)) {
            case 3:
                get_or_add_temp_var(q.num2);
            case 2:
                get_or_add_temp_var(q.tar);
            case 1:
                get_or_add_temp_var(q.num1);
            case 0:
                break;
        }

        switch(q.op) {
            case OP::PLUS:
                genop("ADD", q.num1, q.num2, q.tar);
                break;
            case OP::MINUS:
                genop("SUB", q.num1, q.num2, q.tar);
                break;
            case OP::MULTPLY:break;
            case OP::DIVIDE:break;
            case OP::MOD:break;
            case OP::INC:
                genop("INC", q.num1, q.tar);
                break;
            case OP::DEC:
                genop("DEC", q.num1, q.tar);
                break;
            case OP::BIT_AND:
                genop("AND", q.num1, q.num2, q.tar);
                break;
            case OP::BIT_OR:
                genop("OR", q.num1, q.num2, q.tar);
                break;
            case OP::BIT_XOR:
                genop("XOR", q.num1, q.num2, q.tar);
                break;
            case OP::SHIFT_LEFT:
                genop("SHL", q.num1, q.num2, q.tar);
                break;
            case OP::SHIFT_RIGHT:
                genop("SHR", q.num1, q.num2, q.tar);
                break;
            case OP::ASSIGN:
                ld<A>(q.num1);
                st<A>(q.tar);
                break;
            case OP::GET_ADDR:break;
            case OP::GET_VALUE:break;
            case OP::CALL:
                // code("CALL " + "" );
                break;
            case OP::RET:break;
            case OP::PUSH:break;
            case OP::POP:break;
            case OP::NEG:break;
            case OP::POS:break;
            case OP::NOT:break;
            case OP::INV:break;
            case OP::LESS_THAN:break;
            case OP::GREATER_THEN:break;
            case OP::LESS_EQUAL:break;
            case OP::GREATER_EQUAL:break;
            case OP::EQUAL:break;
            case OP::NOT_EQUAL:break;
            case OP::LOG_AND:break;
            case OP::LOG_OR:break;
            case OP::FUNC:
                code(ST[q.num1].name + " PROC");
                break;
            case OP::EFUNC:
                code(ST[q.num1].name + " EPROC");
                break;
            case OP::IF:break;
            case OP::EI:break;
            case OP::EL:break;
            case OP::WH:break;
            case OP::DO:break;
            case OP::EW:break;
            case OP::FOR:break;
            case OP::JIF:break;
            case OP::EFOR:break;
            case OP::NOP:break;
            case OP::JMP:break;
            case OP::DEF_FUN:break;
            case OP::DEF_VAR:
                local_var_size += ST.get_type_size(q.num1);
                local_temp_size = local_var_size;
                break;
            case OP::LABEL:break;
            case OP::GOTO:break;
            case OP::CONTINUE:break;
            case OP::BREAK:break;
            case OP::INDEX:break;
            case OP::DEF_STRU:break;
            case OP::DEF_STRU_ELE:break;
            case OP::EDEF_STRU:break;
            case OP::MEMBER:break;
            case OP::PMEMBER:break;
            case OP::RETN:break;
            case OP::ASM:break;
        }
    }
}