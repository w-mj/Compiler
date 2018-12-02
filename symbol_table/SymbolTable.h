//
// Created by wmj on 18-11-29.
//

#ifndef COMPLIE_SYMBOLTABLE_H
#define COMPLIE_SYMBOLTABLE_H

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "../word_analysis/NumericDFA.h"

//#define CHAR 0x01
//#define LONG 0x02
//#define ARRAY 0x04
//#define FUNCTION 0x08
//#define INT 0x10
//#define ENUM 0x20
//#define STRUCT 0x40
//#define CONST 0x80
//#define UNSIGNED 0x100
//#define FLOAT 0x200
//#define UNION 0x400
//#define TYPE 0x800
//#define SHORT 0x1000
//#define DOUBLE 0x2000
//#define POINTER 0x4000
//#define VOID 0x8000


class SymbolTable {

public:
    SymbolTable();

    enum class Type_op {
        CHAR, LONG, ARRAY, FUNCTION, INT, ENUM, STRUCT, CONST,
        UNSIGNED, FLOAT, UNION, SHORT, DOUBLE, POINTER, VOID, LABEL, TYPE
    };
    struct Type {
        std::string name;
        Type_op op;
        size_t size;
        size_t index;
    };

    struct Symbol {
        std::string name;
        size_t type_index;
        size_t index;
        uint offset;
    };


    struct Array {
        size_t len;
        size_t type_index;
    };

    struct Struct {
        std::string name;
        uint offset;
        size_t type_index;
    };

    struct Function {
        uint level;
        uint param_num;
        size_t param_index;
        uint entry;
    };

    struct Constant {
        std::string name;
        Type_op type;
        size_t type_index;
        size_t index;
    };

    struct Label {
        uint offset;
    };

    struct Table {
        std::vector<SymbolTable::Symbol> symbol_list;
        std::map<std::string, size_t> symbol_index;
        Table* up = nullptr;
    } top_table;

    std::vector<Type> type_list;
    std::map<std::string, size_t> type_index;
    std::vector<Struct> struct_member_list;
    std::vector<Struct> func_param_list;
    std::vector<Number> constant_num_list;
    std::vector<Label> label_list;

};



#endif //COMPLIE_SYMBOLTABLE_H
