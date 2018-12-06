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

#define SYM(x) (symbol_list[(x)])
#define TYPE(x) (type_list[SYM(x).type])
#define STRU(x) (struct_list[TYPE(x).data])
#define ARR(x) (array_list[TYPE(x).data])
#define FUNC(x) (function_list[TYPE(x).data])


using namespace std;
SymbolTable::SymbolTable()
{
    current_table = top_table = new Table();
    get_or_add_type({INT, INT_SIZE, 0});
    get_or_add_type({SHORT, SHORT_SIZE, 0});
    get_or_add_type({LONG, LONG_SIZE, 0});
    get_or_add_type({FLOAT, FLOAT_SIZE, 0});
    get_or_add_type({DOUBLE, DOUBLE_SIZE, 0});
    get_or_add_type({POINTER, POINTER_SIZE, 0});
    add_symbol({"int", top_table->type_index[{INT, INT_SIZE, 0}], Cat_Type, 0});
    add_symbol({"short", top_table->type_index[{SHORT, SHORT_SIZE, 0}], Cat_Type, 0});
    add_symbol({"long", top_table->type_index[{LONG, LONG_SIZE, 0}], Cat_Type, 0});
    add_symbol({"float", top_table->type_index[{FLOAT, FLOAT_SIZE, 0}], Cat_Type, 0});
    add_symbol({"double", top_table->type_index[{DOUBLE, DOUBLE_SIZE, 0}], Cat_Type, 0});
}

size_t SymbolTable::get_or_add_type(const Type&& type) {
    if (in_set(current_table->type_index, type))
        return current_table->type_index[type];
    size_t i = type_list.size();
    type_list.push_back(type);
    current_table->type_index.emplace(type_list[i], i);
    return i;
}

const SymbolTable::Symbol *SymbolTable::get_symbol_by_name(const std::string &name) {
    auto c = current_table;
    while (c != nullptr) {
        if (in_set(c->symbol_index, name))
            return &symbol_list[c->symbol_index[name]];
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

size_t SymbolTable::add_symbol(const Symbol& s) {
    if (get_symbol_by_name(s.name) != nullptr) {
        error("Symbol " + s.name + " is already installed.");
        return nullptr;
    }
    size_t t = symbol_list.size();
    symbol_list.push_back(s);
    current_table->symbol_index.emplace(s.name, t);
    return t;
}

SymbolTable::Symbol &SymbolTable::operator[](size_t i) {
    return symbol_list[i];
}

size_t SymbolTable::get_type_index_by_symbol(size_t i) {
    return symbol_list[i].type;
}


string SymbolTable::get_temp_var_name(const string& suffix) {
    return "@" + to_string(temp_cnt++) + "_" + suffix;
}

size_t SymbolTable::get_struct_field_by_symbol_filed(size_t s, size_t f) {
    return SYM(f).type;
}

size_t SymbolTable::get_array_element_type(size_t symbol) {
    return array_list[TYPE(symbol).data].type;
}

SymbolTable::Type &SymbolTable::get_type_by_symbol(size_t i) {
    return type_list[symbol_list[i].type];
}


size_t SymbolTable::get_function_type(size_t symbol) {
    return FUNC(symbol).ret_type;
}

size_t SymbolTable::add_constant_Symbol(const Number &num) {
    long f = find(constant_num_list.begin(), constant_num_list.end(), num) - constant_num_list.begin();
    if (f >= constant_num_list.size())
        constant_num_list.push_back(num);
    auto t = get_symbol_index_by_name("const_" + num.str());
    if (t == -1)
        return add_symbol({"const_" + num.str(), get_or_add_type({CONST, 0, f}), Cat_Const, 0});
    return static_cast<size_t>(t);
}

int SymbolTable::get_symbol_index_by_name(const std::string &name) {
    auto c = current_table;
    while (c != nullptr) {
        if (in_set(c->symbol_index, name))
            return static_cast<int>(c->symbol_index[name]);
        c = c->up;
    }
    return -1;
}

size_t SymbolTable::get_type_size(size_t symbol) {
    return TYPE(symbol).size;
}
