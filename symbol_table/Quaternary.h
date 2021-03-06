//
// Created by wmj on 18-12-5.
//

#ifndef COMPLIE_QUATERNARY_H
#define COMPLIE_QUATERNARY_H


#include <cstddef>
#include "SymbolTable.h"

#define QL QuatList::getInstance()


enum class OP {
    PLUS=0, MINUS, MULTPLY, DIVIDE, MOD, INC, DEC,
    BIT_AND=7, BIT_OR, BIT_XOR, SHIFT_LEFT, SHIFT_RIGHT,
    ASSIGN=12,
    GET_ADDR=13, GET_VALUE,
    CALL=15, RET, PUSH, POP,
    NEG=19, POS, NOT, INV, // NOT: !, INV: ~
    LESS_THAN=23, GREATER_THEN, LESS_EQUAL, GREATER_EQUAL, EQUAL, NOT_EQUAL, LOG_AND, LOG_OR,
    FUNC=31, EFUNC, IF, EI, EL, WH=36, DO, EW, FOR, JIF, EFOR,
    NOP=42, JMP, DEF_FUN, DEF_VAR, LABEL, GOTO, CONTINUE, BREAK,
    INDEX=50, DEF_STRU, DEF_STRU_ELE, EDEF_STRU, MEMBER, PMEMBER, RETN, ASM
};

struct Quat {
    OP op;
    size_t num1;
    size_t num2;
    size_t tar;
    Quat(OP op, size_t num1, size_t num2, size_t tar): op(op), num1(num1), num2(num2), tar(tar) {}
};

class QuatList {
    std::vector<Quat> quat_list {Quat(OP::NOP, 0 ,0, 0)};
    QuatList() = default;;
public:
    QuatList(QuatList& q) = delete;
    QuatList& operator==(QuatList&) = delete;
    static QuatList& getInstance() {
        static QuatList ins;
        return ins;
    }
    template<typename... _Args>
    size_t emplace_back(_Args...args) {
        size_t i = quat_list.size();
        quat_list.emplace_back(args...);
        return i;
    }
    Quat& operator[](size_t i);
    size_t size();
    void clear();

    std::vector<Quat>& get_vector();

    friend std::ostream& operator<<(std::ostream& os, QuatList& ql);
};

std::ostream& operator<<(std::ostream& os, QuatList& ql);



// 生成一个四元式和临时变量，并将临时变量在符号表中的索引返回
//size_t quat(OP op, size_t num1, size_t num2);
size_t quat(OP op, size_t num1, size_t, size_t target=0);

size_t make_unary_operator_quat(void* it, size_t num1);
size_t make_assign_operator_quat(size_t l, size_t a, size_t r);
size_t make_struct_member_quat(size_t str, std::string* menber);

std::string op_to_str(OP op);


#endif //COMPLIE_QUATERNARY_H
