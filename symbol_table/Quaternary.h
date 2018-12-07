//
// Created by wmj on 18-12-5.
//

#ifndef COMPLIE_QUATERNARY_H
#define COMPLIE_QUATERNARY_H


#include <cstddef>
#include "SymbolTable.h"


enum class OP {
    PLUS, MINUS, MULTPLY, DIVIDE, MOD, INC, DEC,
    BIT_AND, BIT_OR, BIT_XOR, SHIFT_LEFT, SHIFT_RIGHT,
    ASSIGN,
    GET_ADDR, GET_VALUE,
    CALL, RET, PUSH, POP,
    NEG, POS, NOT, INV, // NOT: !, INV: ~
    LESS_THAN, GREATER_THEN, LESS_EQUAL, GREATER_EQUAL, EQUAL, NOT_EQUAL, LOG_AND, LOG_OR
    FUNC, EFUNC, IF, EI, EL, WH, DO, EW
};


// 生成一个四元式和临时变量，并将临时变量在符号表中的索引返回
size_t quat(OP op, size_t num1, size_t num2);
size_t quat(OP op, size_t num1, size_t, size_t target);

size_t make_unary_operator_quat(void* it, size_t num1);
size_t make_assign_operator_quat(size_t l, size_t a, size_t r);


#endif //COMPLIE_QUATERNARY_H
