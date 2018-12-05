//
// Created by wmj on 18-11-29.
//

#include "SymbolTable.h"
#include "../Utility.h"
#include "../error/Error.h"

#define SHORT_SIZE 1
#define INT_SIZE 2
#define LONG_SIZE 2
#define POINTER_SIZE 4
#define FLOAT_SIZE 4
#define DOUBLE_SIZE 4

using namespace std;
SymbolTable::SymbolTable()
{
    current_table = top_table = new Table();
    get_or_add_type({INT, INT_SIZE});
    get_or_add_type({SHORT, SHORT_SIZE});
    get_or_add_type({LONG, LONG_SIZE});
    get_or_add_type({FLOAT, FLOAT_SIZE});
    get_or_add_type({DOUBLE, DOUBLE_SIZE});
    get_or_add_type({POINTER, POINTER_SIZE});
}

size_t SymbolTable::get_or_add_type(const Type&& type) {
    if (in_set(current_table->type_index, type))
        return current_table->type_index[type];
    size_t i = current_table->type_list.size();
    current_table->type_list.push_back(type);
    current_table->type_index.emplace(current_table->type_list[i], i);
    return i;
}

const SymbolTable::Symbol *SymbolTable::get_symbol_by_name(const std::string &name) {
    auto c = current_table;
    while (c != nullptr) {
        if (in_set(c->symbol_index, name))
            return &c->symbol_list[c->symbol_index[name]];
        c = c->up;
    }
    return nullptr;
}

void SymbolTable::in() {
    auto c = new Table;
    c->up = current_table;
    current_table = c;
}

void SymbolTable::leave() {
    auto t = current_table;
    current_table = current_table->up;
    delete t;
}

SymbolTable::~SymbolTable() {
    while (current_table != nullptr) {
        auto t = current_table;
        current_table = current_table->up;
        delete t;
    }
}

const SymbolTable::Symbol *SymbolTable::add_symbol(const std::string &name, const Type &type, size_t data) {
    if (get_symbol_by_name(name) != nullptr) {
        error("Symbol " + name + " is already installed.");
        return nullptr;
    }
    size_t t = current_table->symbol_list.size();
    current_table->symbol_list.emplace_back(name, type, data);
    current_table->symbol_index.emplace(name, t);
    return &current_table->symbol_list[t];
}

const SymbolTable::Symbol *SymbolTable::add_symbol(const Type &type, size_t data) {
    return add_symbol(to_string(temp_cnt++), type, data);
}
