//
// Created by wmj on 18-11-29.
//

#ifndef COMPLIE_SYMBOLTABLE_H
#define COMPLIE_SYMBOLTABLE_H

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <atomic>

#include "../word_analysis/NumericDFA.h"
#include "type.h"
#include "../word_analysis/TokenList.h"

#define SYMBOL_

typedef Token Item;

class SymbolTable {
private:
    SymbolTable();
public:
    ~SymbolTable();

    static SymbolTable& getInstance()
    {
        static SymbolTable instance;
        return instance;
    }

    struct Symbol {
        std::string name;
        size_t type;
        size_t data;
    };


    struct Array {
        size_t len;
        size_t type;
    };

    struct Struct {
        size_t num_fields;
        size_t field;
        struct Field {
            std::string name;
            size_t type;
            size_t offset;
        };
    };

    struct Function {
        uint param_num;
        size_t param_index;
        uint entry;
        struct Param {
            size_t type;
            size_t offset;
        };
    };

    struct Label {
        uint offset;
    };

    struct Table {
        std::vector<SymbolTable::Symbol> symbol_list;
        std::map<std::string, size_t> symbol_index;
        std::vector<Type> type_list;
        std::map<Type, size_t> type_index;

        std::vector<Struct> struct_list;
        std::vector<SymbolTable::Struct::Field> strcut_field;
        std::vector<SymbolTable::Function::Param> func_param_list;
        std::vector<SymbolTable::Array> array_list;

        std::vector<Number> constant_num_list;
        std::vector<Label> label_list;

        Table* up = nullptr;
    };

private:
    Table* top_table;
    Table* current_table;
    int temp_cnt = 0;
public:

    size_t get_or_add_type(const Type&& type);
    const Symbol* get_symbol_by_name(const std::string& name);
    const Symbol* add_symbol(const std::string& name, const Type& type, size_t data = 0);
    const Symbol* add_symbol(const Type& type, size_t data = 0);

    void in();
    void leave();

};



#endif //COMPLIE_SYMBOLTABLE_H
