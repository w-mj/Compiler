//
// Created by wmj on 18-12-22.
//

#include "80862.h"
#include "../symbol_table/SymbolTable.h"
#include "../symbol_table/Quaternary.h"
#include <iostream>
#include <string>
#include <map>

using namespace std;

void pasm(const string& op, const string &n1="", const string& n2="") {
    cout << op << " " << n1 << " " << n2 << endl;
}

map<size_t, size_t> temp_var_table;
size_t local_var_size;
size_t local_temp_size;
size_t get_or_add_temp_var(size_t  n) {
    if (temp_var_table.find(n) == temp_var_table.end()) {
        temp_var_table.emplace(n, local_temp_size);
        local_temp_size += ST.get_type_size(n);
        ST[n].offset = static_cast<int>(local_temp_size);
    }
    return temp_var_table[n];
}

string get_var(size_t t) {
//    if (ST[t].offset == 0)
//        get_or_add_temp_var(t);
//    if (ST[t].is_addr) {
//        return "[BP + " + to_string(ST[t].offset) + "]";
//    } else
return "";
}

void lda(size_t t) {
    if (ST[t].is_addr) {
        if (ST.get_type_size(t) == 1)
            cout << "MOV AL [BP + " + to_string(ST[t].offset) + "]";
        else
            cout << "MOV AX [BP + " + to_string(ST[t].offset) + "]";
    }
}

void make8086() {
    for (const Quat& q: QL.get_vector()) {
        switch(q.op) {
            case OP::PLUS:break;
            case OP::MINUS:break;
            case OP::MULTPLY:break;
            case OP::DIVIDE:break;
            case OP::MOD:break;
            case OP::INC:break;
            case OP::DEC:break;
            case OP::BIT_AND:break;
            case OP::BIT_OR:break;
            case OP::BIT_XOR:break;
            case OP::SHIFT_LEFT:break;
            case OP::SHIFT_RIGHT:break;
            case OP::ASSIGN:break;
            case OP::GET_ADDR:break;
            case OP::GET_VALUE:break;
            case OP::CALL:break;
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
            case OP::FUNC:break;
            case OP::EFUNC:break;
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