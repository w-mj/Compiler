//
// Created by wmj on 18-12-4.
//

#ifndef COMPLIE_TYPE_H
#define COMPLIE_TYPE_H

/**
 * 此头文件用于定义和描述数据类型
 *
 * struct Type {
 *      uint8 type;
 *      size_t data_index;
 * };
 *
 * type由七个二进制进制位描述，保存在uint8中
 * ------------------------
 * |A|C|U|P|s|type|
 *  7 6 5 4 3 2  0
 * ------------------------
 * s位为0时为基本数据类型，为1时为特殊数据类型
 * 特殊类型中包括Type、Label、Array、Function、Struct、Union、Enum
 * 基本类型包括Char、Long、Int、Float、Short、Double、Void
 * 特殊数据类型时，由data_index指向相应的数据单元，基本类型中的data_index无意义。
 * 当ADDRESS被置位时，说明该变量中存的是地址，需要寻址后再计算
 */


#include <cstdlib>
#include <tiff.h>

#define ADDRESS 0x80
#define CONST 0x40
#define UNSIGNED 0x20
#define POINTER 0x10

#define CHAR 0x01
#define LONG 0x02
#define INT 0x03
#define FLOAT 0x04
#define SHORT 0x05
#define DOUBLE 0x06
#define VOID 0x07

#define TYPE 0x09
#define LABEL 0x0A
#define ARRAY 0x0B
#define FUNCTION 0x0C
#define STRUCT 0x0D
#define UNION 0x0E
#define ENUM 0x0F

struct Type {
    const uint8 t;
    const size_t data;
    bool operator==(const Type& an) const {
        return t == an.t && data == an.data;
    }
    bool operator<(const Type& an) const {
        return t + data < an.t + an.data;
    }
};



#endif //COMPLIE_TYPE_H
