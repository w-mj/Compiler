//
// Created by wmj on 18-11-29.
//

#include <stack>
#include <cstdio>
#include <cstdlib>
#include "SymbolTable.h"
#include "../Utility.h"
#include "../error/Error.h"
#include "../word_analysis/WordAnalysis.h"

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
    type_list.emplace_back(0, 0, 0); // 0 号不用，表示“没有类型”或“不完全类型”
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

size_t SymbolTable::get_or_add_type(const Type& type) {
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
    // cout << "Symbol table in" << endl;
}

void SymbolTable::leave() {
    auto t = current_table;
    current_table = current_table->up;
    delete t;
    // cout << "Symbol table leave" << endl;
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
        error("Symbol " + s.name + " is already exists.");
        return 0;
    }

    size_t t = symbol_list.size();
    symbol_list.push_back(s);
    if (s.cat == Cat_Var) {
        last_vec(symbol_list).offset = current_table->offset;
        current_table->offset += type_list[s.type].size;
    }
    // 为类型和普通变量添加索引
    if (s.cat == Cat_Type || s.cat == Cat_Var || s.cat == Cat_Const)
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
    int t = get_symbol_index_by_name_without_error("@const_" + num.str());
    if (t == -1)
        return add_symbol({"@const_" + num.str(), get_or_add_type({CONST, 0, f}), Cat_Const, 0});
    return static_cast<size_t>(t);
}

bool SymbolTable::is_symbol_exists(const std::string& name) {
    auto c = current_table;
    while (c != nullptr) {
        if (in_set(c->symbol_index, name))
            return true;
        c = c->up;
    }
    return false;
}

int SymbolTable::get_symbol_index_by_name_without_error(const std::string& name) {
    auto c = current_table;
    while (c != nullptr) {
        if (in_set(c->symbol_index, name))
            return static_cast<int>(c->symbol_index[name]);
        c = c->up;
    }
    return -1;
}

size_t SymbolTable::get_symbol_index_by_name(const std::string &name) {
    auto c = current_table;
    while (c != nullptr) {
        if (in_set(c->symbol_index, name))
            return c->symbol_index[name];
        c = c->up;
    }
    error("identification " + name + " is not defined.");
    return 0;
}

size_t SymbolTable::get_type_size(size_t symbol) {
    return TYPE(symbol).size;
}

size_t SymbolTable::set_variables_type(void *tv, void *vv, int cat) {
    auto last_type = (Type*)tv;
    auto last_t_index = get_or_add_type(*last_type);

    auto * variable_list = (vector<size_t>*)vv;
    for (auto i: *variable_list) {
        ST[i].cat=cat;
        auto t = ST[i].type;
        if (t == 0) {
            ST[i].type = last_t_index;
        }
        else {
            // 找到最底层类型
            while (t != 0) {
                switch (type_list[t].t) {
                    case ARRAY:
                        t = array_list[type_list[t].data].type;
                        if (t == 0)
                            array_list[type_list[t].data].type = last_t_index;
                        break;
                    default:
                        t = type_list[t].data;
                        if (t == 0)
                            type_list[t].data = last_t_index;
                }
            }
        }
    }
    return (*variable_list)[0];
}

size_t SymbolTable::add_struct_or_union(size_t struct_or_union, void* name, size_t declaration_list) {
    bool is_struct = WA.key[struct_or_union] == "struct";
    int symbol = get_symbol_index_by_name(*((string*)name));  // TODO: 重定义错误
    if (symbol == -1)
        symbol = add_symbol({*((string*)name), 0, Cat_Var});

    TYPE(symbol).data = struct_list.size();
    struct_list.emplace_back(current_table->symbol_index.size(), declaration_list);

    if (is_struct) {
        for (auto i = 0; i < STRU(symbol).num_fields; i++)
            TYPE(symbol).size += TYPE(i + declaration_list).size;
        TYPE(symbol).t = STRUCT;
    } else {
        for (auto i = 0; i < STRU(symbol).num_fields; i++)
            TYPE(symbol).size = max(TYPE(symbol).size, TYPE(i + declaration_list).size);
        TYPE(symbol).t = UNION;
    }
    return symbol;
}

size_t SymbolTable::add_struct_or_union(size_t struct_or_union, size_t declaration_list) {
    string name = get_temp_var_name();
    return add_struct_or_union(struct_or_union, &name, declaration_list);
}

size_t SymbolTable::get_or_add_array(const SymbolTable::Array &array) {
    if (array_map.find(array) == array_map.end()) {
        array_map.emplace(array, array_list.size());
        array_list.push_back(array);
    }
    return array_map[array];
}

void* TypeBuilder::add_storage(size_t key_in_token, void* t) {
    warring("storage class specify is not supported.");
}

void* TypeBuilder::add_qulifier(size_t key_in_token, void* t) {
    if (WA.key[key_in_token] == "const")
        ((SymbolTable::Type*)t)->cst = true;
    return t;
}


void *TypeBuilder::add_speicifer(void *it, void *t) {
    auto i = (Item*)it;
    auto ty = (SymbolTable::Type*)t;
    if (ty == nullptr)
        ty = new SymbolTable::Type{0, 0, 0};
    // TODO: 检测类型名重复
    if (i->first == 'k') {
        // 内置类型
        const auto& k = WA.key[i->second];
        if (k == "int") {
            ty->t = INT;
            ty->size = INT_SIZE;
        }
        else if (k == "short") {
            ty->t = SHORT;
            ty->size = SHORT_SIZE;
        } else if (k == "long") {
            ty->t = LONG;
            ty->size = LONG_SIZE;
        }
         else if (k == "float") {
            ty->t = FLOAT;
            ty->size = FLOAT_SIZE;
        }
        // TODO: OTHERS
    } else {
        // 自定义类型
        auto& defined_type = ST.get_type_by_symbol(i->second);  // 取类型
        ty->t = defined_type.t;
        ty->size = defined_type.size;
        ty->data = defined_type.data;
    }
    return ty;

}

size_t SymbolTable::TempSymbol::insert_type_into_table(size_t ti) {
    switch (tl[ti].t) {
        case ARRAY:
            tl[ti].data = insert_array_into_table(tl[ti].data);
            break;
        default:
            throw runtime_error(debugpos + " not support type");
    }
    return ST.get_or_add_type(tl[ti]);
}

size_t SymbolTable::TempSymbol::insert_array_into_table(size_t ai) {
    if (al[ai].type != 0)
        al[ai].type = insert_array_into_table(al[ai].type);
    return ST.get_or_add_array(al[ai]);
}

size_t SymbolTable::TempSymbol::insert_symbol_into_table(int cat) {
    s.cat = cat;
    if (s.type != 0)
        s.type = insert_type_into_table(s.type);
    else
        s.type = ST.get_or_add_type(tl[s.type]);
    return ST.add_symbol(s);
}

SymbolTable::TempSymbol *SymbolTable::TempSymbol::add_array(size_t len) {
    // len 是一个constant expression计算的结果
    uint32_t lenn = ST.constant_num_list[ST.type_list[ST.symbol_list[len].type].data].value.ui;
    last_vec(tl).t = ARRAY;
    last_vec(tl).data = al.size();
    al.emplace_back(lenn, tl.size());
    tl.emplace_back(0, 0, 0);
    return this;
}

SymbolTable::TempSymbol * SymbolTable::TempSymbol::add_array() {
    last_vec(tl).t = ARRAY;
    last_vec(tl).data = al.size();
    al.emplace_back(0, tl.size());
    tl.emplace_back(0, 0, 0);
    return this;
}

SymbolTable::TempSymbol *
SymbolTable::TempSymbol::merge_pointer(std::vector<SymbolTable::Type> *pointer_ype_list) {
    auto t = s.type;
    while (t != 0) {
        switch (tl[t].t) {
            case ARRAY:
                t = al[tl[t].data].type;
                if (t == 0)
                    al[tl[t].data].type = tl.size();
                break;
            default:
                t = tl[t].data;
                if (t == 0)
                    tl[t].data = tl.size();
        }
    }
    for (auto i = 0; i < pointer_ype_list->size(); i++) {
        tl.push_back((*pointer_ype_list)[i]);
        (*pointer_ype_list)[i].data = tl.size();
    }
    return this;
}

SymbolTable::TempSymbol *SymbolTable::TempSymbol::add_basic_type(SymbolTable::Type &type) {
    if (tl.empty())
        tl.push_back(type);
    else
        last_vec(tl) = type;
    return this;
}

size_t SymbolTable::TempSymbol::add_basic_type_and_insert_into_table(vector<SymbolTable::TempSymbol *> v,
                                                                     SymbolTable::Type *t, int cat) {
    size_t k = -1;
    for (auto x: v) {
        x->add_basic_type(*t);
        if (k == -1)
            k = x->insert_symbol_into_table(cat);
        else
            x->insert_symbol_into_table(cat);
        delete x;
    }
    return k;
}


std::ostream& operator<<(std::ostream& os, SymbolTable::Symbol& s) {
    os << s.name << ":" << ST.type_list[s.type];
    if (s.cat == Cat_Var)
        os << "&" << s.offset;
    if (s.cat == Cat_Type)
        os << "*" << ST.type_list[s.type].size;
    return os;
}

std::ostream& operator<<(std::ostream& os, SymbolTable::Array& a) {
    os <<  ST.type_list[a.type] << "[" << a.len << "]";
    return os;
}

std::ostream& operator<<(std::ostream& os, SymbolTable::Type& t) {
    if (t.cst)
        os << "const ";
    switch (t.t) {
        case INT:
            os << "int ";
            break;
        case SHORT:
            os << "short ";
            break;
        case FLOAT:
            os << "float ";
            break;
        case DOUBLE:
            os << "double ";
            break;
        case LONG:
            os << "long ";
            break;
        case ARRAY:
            os << ST.array_list[t.data];
            break;
        case CONST:
            os << ST.constant_num_list[t.data];
            break;
        default:
            os << "unknown type";
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, SymbolTable& st) {
    for (size_t i = 0; i < st.symbol_list.size(); i++) {
        os << i << "#" << st.symbol_list[i] << endl;
    }
    return os;
}

std::string SymbolTable::get_symbol_name(size_t symbol) {
    return ST[symbol].name;
}

bool SymbolTable::is_temp_var(size_t symbol) {
    return ST[symbol].name[0] == '@' && isdigit(ST[symbol].name[1]);
}

void SymbolTable::set_symbol_addr(size_t symbol, size_t addr) {
    ST[symbol].addr = addr;
}

size_t SymbolTable::get_symbol_addr(size_t symbol) {
    return ST[symbol].addr;
}

int SymbolTable::get_symbol_offset(size_t symbol) {
    return ST[symbol].offset;
}

int SymbolTable::get_func_param_num(size_t symbol) {
    return ST.function_list[ST.type_list[ST[symbol].type].data].param_num;
}

std::string SymbolTable::get_type_name(size_t symbol) {
    switch(get_basic_symbol_type(symbol)) {
        case INT:
            return "int";
        case SHORT:
            return "short";
        case LONG:
            return "long";
        case DOUBLE:
            return "double";
        case FLOAT:
            return "float";
        default:
            throw runtime_error("<SymbolTable.cpp get_type_name> not support type name.");
    }
}

size_t SymbolTable::recursive_type(size_t type) {
    switch (type_list[type].t) {
        case INT:
            return get_symbol_by_name("int")->type;
        case FLOAT:
            return get_symbol_by_name("float")->type;
        case SHORT:
            return get_symbol_by_name("short")->type;
        case LONG:
            return get_symbol_by_name("long")->type;
        case DOUBLE:
            return get_symbol_by_name("double")->type;
        case ARRAY:
            return recursive_type(array_list[type_list[type].data].type);
        case FUNCTION:
            return recursive_type(function_list[type_list[type].data].ret_type);
        default:
            throw runtime_error("<SymbolTable.cpp recursive_type> not support type " + type);
    }
}

size_t SymbolTable::get_basic_symbol_type(size_t symbol) {
    switch (TYPE(symbol).t) {
        case INT:
            return get_symbol_by_name("int")->type;
        case FLOAT:
            return get_symbol_by_name("float")->type;
        case SHORT:
            return get_symbol_by_name("short")->type;
        case LONG:
            return get_symbol_by_name("long")->type;
        case DOUBLE:
            return get_symbol_by_name("double")->type;
        case ARRAY:
            return recursive_type(ARR(symbol).type);
        case FUNCTION:
            return recursive_type(FUNC(symbol).ret_type);
        default:
            throw runtime_error("<SymbolTable.cpp get_basic_symbol_type> not support type " + TYPE(symbol).t);
    }
}
