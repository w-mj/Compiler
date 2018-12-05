//
// Created by wmj on 18-12-5.
//

#ifndef COMPLIE_QUATERNARY_H
#define COMPLIE_QUATERNARY_H

enum class OP {
    PLUS, MINUS, MULTPLY, DIVIDE, MOD,
    AND, OR, XOR, ASSIGN, GET_ADDR, GET_VALUE,
    ARRAY_INDEX,
    CALL, RET
};

void quat(OP op, int num1, int num2, int target=-1);

#endif //COMPLIE_QUATERNARY_H
