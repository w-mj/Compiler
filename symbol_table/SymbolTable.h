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

#define CONSTANT 1
#define LABELS 2
#define GLOBAL 3
#define PARAM 4
#define LOCAL 5


class SymbolTable {

public:
    SymbolTable();

    enum class Type_op {
        CHAR, LONG, ARRAY, FUNCTION, INT, ENUM, STRUCT, CONST,
        UNSIGNED, FLOAT, UNION, VOLATILE, SHORT, DOUBLE, POINTER, VOID
    };
    struct Type {
        Type_op op;
        size_t size;
    };

    struct Type;

    struct Array {
        size_t len;
        Type type;
    };

    struct Field {
        size_t offset;
        Type type;
    };

    struct Struct {
        std::vector<Field> fileds;
    };

    struct Function {
        uint level;
        uint offset;
        std::vector<Field> params;
    };

    using Value =  union {
        unsigned short us;
        signed short ss;
        unsigned int ui;
        signed int si;
        unsigned long ul;
        signed long sl;
        float ft;
        double db;
        char sc;
        void *pt;
    };

    struct Symbol {
        std::string name;
        uint scope;
        enum class Scalss {
            AUTO, REGISTER, STATIC, EXTERN
        } scalss = Scalss::AUTO;
        bool temp;
        bool generated;  // 无名变量为true
        Type type;
        union {
            struct {
                int label;
                std::weak_ptr<Symbol> equated_to;
            } l;
            // <struct types 50>
            // <enum constants 52>
            // <enum types 52>
            struct {
                Value v;
                std::weak_ptr<Symbol> loc;
            } c;
            // <function symbols 226>
            // <globals 206>
            // <temporaries 270>
        } u;
    };

    struct Table {
        uint level;
        std::map<std::string, size_t> table;  // 仅保存索引
        std::vector<std::shared_ptr<Symbol>> all_symbol;  // symbol 列表
        std::shared_ptr<Table> previous = nullptr;
    };

    std::shared_ptr<Table> constants;
    std::shared_ptr<Table> externals;
    std::shared_ptr<Table> globals;
    std::shared_ptr<Table> identifiers;
    std::shared_ptr<Table> labels;
    std::shared_ptr<Table> types;

    std::shared_ptr<Table> make_table(const std::shared_ptr<Table>& base, uint level);

    std::shared_ptr<Symbol> install(const std::string& name, std::shared_ptr<Table> tpp, uint level);

};



#endif //COMPLIE_SYMBOLTABLE_H
