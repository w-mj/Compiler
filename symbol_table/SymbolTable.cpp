//
// Created by wmj on 18-11-29.
//

#include <stack>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include "SymbolTable.h"
#include "../Utility.h"
#include "../error/Error.h"
#include "../word_analysis/WordAnalysis.h"
#include "Quaternary.h"

#define SHORT_SIZE 1
#define CHAR_SIZE 1
#define INT_SIZE 2
#define LONG_SIZE 4
#define POINTER_SIZE 2
#define FLOAT_SIZE 2
#define DOUBLE_SIZE 4

#define SYM(x) (symbol_list[(x)])
#define TYPE(x) (type_list[SYM(x).type])
#define STRU(x) (struct_list[TYPE(x).data])
#define ARR(x) (array_list[TYPE(x).data])
#define FUNC(x) (function_list[TYPE(x).data])

#define cant_be_duplicate(x) (x.cat == Cat_Type || x.cat == Cat_Var)


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
    add_symbol({"@@nothing", 0, Cat_Label, 0});
    add_symbol({"int", top_table->type_index[{INT, INT_SIZE, 0}], Cat_Type, 0});
    add_symbol({"short", top_table->type_index[{SHORT, SHORT_SIZE, 0}], Cat_Type, 0});
    add_symbol({"long", top_table->type_index[{LONG, LONG_SIZE, 0}], Cat_Type, 0});
    add_symbol({"float", top_table->type_index[{FLOAT, FLOAT_SIZE, 0}], Cat_Type, 0});
    add_symbol({"double", top_table->type_index[{DOUBLE, DOUBLE_SIZE, 0}], Cat_Type, 0});
    Number True{};
    True.type = Number::Short;
    True.value.si = 1;
    add_constant_Symbol(True);
}

size_t SymbolTable::get_or_add_type(const Type& type) {
//    long t = find(type_list.begin(), type_list.end(), type) - type_list.begin();
//    if (t < type_list.size())
//        return static_cast<size_t>(t);
    Table* t = current_table;
    while (t != nullptr) {
        if (in_set(t->type_index, type))
            return t->type_index[type];
        t = t->up;
    }
    size_t i = type_list.size();
    type_list.push_back(type);
    if (type.t == ARRAY)
        last_vec(type_list).size = array_list[type.data].len * type_list[array_list[type.data].type].size;
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
    if (current_table->next_func) {
        current_table->next_func = false;
        return;
    }
    auto c = new Table;
    c->up = current_table;
    current_table = c;

    c->offset = c->up->offset;
    // cout << "Symbol table in" << endl;
}

void SymbolTable::in(std::map<std::string, size_t> symbol_index) {
    in();
    current_table->symbol_index.insert(symbol_index.begin(), symbol_index.end());
    current_table->offset = 0;
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
    if (cant_be_duplicate(s) && get_symbol_by_name(s.name) != nullptr) {
        error("Symbol " + s.name + " is already exists.");
        return 0;
    }

    if (s.cat == Cat_Param) {
        throw runtime_error(debugpos + "add param by \"add_symbol\"");
    }

    size_t t = symbol_list.size();
    symbol_list.push_back(s);
    // 分配栈偏移
    if (oneof(s.cat, Cat_Var, Cat_Func_Declaration)) {
        last_vec(symbol_list).offset = current_table->offset;
        current_table->offset += type_list[s.type].size;
    }
    // 为类型和普通变量添加索引
    if (oneof(s.cat, Cat_Type, Cat_Var, Cat_Const, Cat_Func_Declaration, Cat_Label, Cat_Func_Defination))
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
    if (t == -1) {
        Type temp_t{0, 0, static_cast<size_t>(f), true};
        switch (num.type) {
            case Number::Int: temp_t.t = INT; break;
            case Number::Short: temp_t.t = SHORT; break;
            case Number::UShort: temp_t.t = SHORT; break;
            case Number::UInt:temp_t.t = INT; break;
            case Number::ULong:temp_t.t = LONG; break;
            case Number::Long:temp_t.t = LONG; break;
            case Number::Float:temp_t.t = FLOAT; break;
            case Number::Double:temp_t.t = DOUBLE; break;
        }
        return add_symbol({"@const_" + num.str(), get_or_add_type(temp_t), Cat_Const, 0});
    }
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
size_t SymbolTable::get_or_add_function(const size_t return_type, const std::vector<size_t>& param_type) {
    for (size_t i = 0; i < function_list.size(); i++) {
        bool equal = true;
        if (function_list[i].ret_type == return_type && function_list[i].param_num == param_type.size()) {
            for (size_t p = 0; equal && p < param_type.size(); p++) {
                if (symbol_list[p].type != param_type[p])
                    equal = false;
            }
            if (equal)
                return i;
        }

    }
    size_t t = function_list.size();
    int param_offset = 0;
    function_list.emplace_back(param_type.size(), return_type, symbol_list.size(), 0);
    for (int i = (int)param_type.size() - 1; i >= 0 ; i--) {
        symbol_list.emplace_back("@func_" + to_string(t) + "_param_" + to_string(i), param_type[i], Cat_Param, param_offset);
        param_offset -= type_list[param_type[i]].size;
    }
    return t;
}

void* TypeBuilder::add_storage(size_t key_in_token, void* t) {
    warring("storage class specify is not supported.");
    return t;
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
        switch (i->second) {
            case kint:
                ty->t = INT;
                ty->size = INT_SIZE;
                break;
            case kshort:
                ty->t = SHORT;
                ty->size = SHORT_SIZE;
                break;
            case klong:
                ty->t = LONG;
                ty->size = LONG_SIZE;
                break;
            case kfloat:
                ty->t = FLOAT;
                ty->size = FLOAT_SIZE;
                break;
            case kdouble:
                ty->t = DOUBLE;
                ty->size = DOUBLE_SIZE;
                break;
            case kvoid:
                ty->t = VOID;
                break;
            default:
                rterr("unsupported type " +  WA.key[i->second]);
        }
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
        case INT:
        case FLOAT:
        case SHORT:
        case LONG:
        case DOUBLE:
        case VOID:
            break;
        case ARRAY:
            tl[ti].data = insert_array_into_table(tl[ti].data);
            break;
        case FUNCTION:
            tl[ti].data = insert_function_into_table(tl[ti].data);
            break;
        default:
            throw runtime_error(debugpos + " not support type");
    }
    return ST.get_or_add_type(tl[ti]);
}

size_t SymbolTable::TempSymbol::insert_array_into_table(size_t ai) {
    al[ai].type = insert_type_into_table(al[ai].type);
    return ST.get_or_add_array(al[ai]);
}

size_t SymbolTable::TempSymbol::insert_function_into_table(size_t fi) {
    fl[fi].ret_type = insert_type_into_table(fl[fi].ret_type);
    vector<size_t> param_type;
    for (size_t i = fl[fi].param_index; i < fl[fi].param_index + fl[fi].param_num; i++) {
        fpl[i]->s.type = fpl[i]->insert_type_into_table(fpl[i]->s.type);
        param_type.push_back(fpl[i]->s.type);
    }
    size_t t = ST.get_or_add_function(fl[fi].ret_type, param_type);
    fl[fi].param_index = ST.get_function_by_index(t).param_index;
    return t;
}

size_t SymbolTable::TempSymbol::insert_symbol_into_table(int cat) {
    s.cat = cat;
    if (oneof(tl[s.type].t, ARRAY, FUNCTION)) {
        s.type = insert_type_into_table(s.type);
    } else
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

SymbolTable::TempSymbol *SymbolTable::TempSymbol::add_function(std::vector<SymbolTable::TempSymbol*>& v) {
    last_vec(tl).t = FUNCTION;
    last_vec(tl).data = fl.size();
    fl.emplace_back(v.size(), tl.size(), 0, 0);
    tl.emplace_back(0, 0, 0);
    fpl.insert(fpl.begin(), v.begin(), v.end());
    return this;
}

SymbolTable::TempSymbol *
SymbolTable::TempSymbol::merge_pointer(std::vector<SymbolTable::Type> *pointer_ype_list) {
    auto t = s.type;
    if (tl[t].t == FUNCTION)
        ST.current_table->next_func = true;
    while (t != 0) {
        switch (tl[t].t) {
            case ARRAY:
                t = al[tl[t].data].type;
                if (t == 0)
                    al[tl[t].data].type = tl.size();
                break;
            case FUNCTION:
                t = fl[tl[t].data].ret_type;
                if (t == 0)
                    fl[tl[t].data].ret_type = tl.size();
                break;
            default:
                t = tl[t].data;
                if (t == 0)
                    tl[t].data = tl.size();
        }
    }
    for (const auto &i : *pointer_ype_list) {
        tl.push_back(i);
        last_vec(tl).data = tl.size();
    }
    tl.emplace_back(0, 0, 0);
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
    auto k = static_cast<size_t>(-1);
    int tcat;
    for (auto x = v.rbegin(); x != v.rend(); x++) {
        k = add_basic_type_and_insert_into_table(*x, t, cat);
    }
    assert(k != -1);
    return k;
}
size_t SymbolTable::TempSymbol::add_basic_type_and_insert_into_table(SymbolTable::TempSymbol* v,
                                                                            SymbolTable::Type* t, int cat) {
    size_t s, q;
    if (cat != Cat_Func_Defination && v->tl[v->s.type].t == FUNCTION)
        cat = Cat_Func_Declaration;
    v->add_basic_type(*t);
    s = v->insert_symbol_into_table(cat);
    switch (cat) {
        case Cat_Func_Declaration:
            q = quat(OP::DEF_FUN, s, 0, 0);
            break;
        case Cat_Var:
            q = quat(OP::DEF_VAR, s, 0, 0);
            break;
        case Cat_Func_Defination:
            q = quat(OP::FUNC, s, 0, 0);
            ST.current_table->next_func = true;
            break;
        default:
            throw runtime_error(debugpos + "don't make quat.");

    }
    if (!v->initializer_list.empty()) {
        if (v->s.cat == Cat_Var) {
            quat(OP::ASSIGN, v->initializer_list[0], 0, s);
        }
    }
    return q;
}

SymbolTable::TempSymbol *SymbolTable::TempSymbol::set_initializer_list(std::vector<size_t> &list) {
    initializer_list = move(list);
    return this;
}

int SymbolTable::TempSymbol::first_type_t() {
    return tl[s.type].t;
}


std::ostream& operator<<(std::ostream& os, SymbolTable::Symbol& s) {
    os << s.name;
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
        case FUNCTION:
            os << ST.function_list[t.data];
            break;
        case VOID:
            os << "void";
            break;
        default:
            os << "unknown type";
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, SymbolTable::Function& f) {
    os << "FUNCTION:" << ST.type_list[f.ret_type] << "->";
    for (size_t i = 0; i < f.param_num; i++)
        os << ST.type_list[ST[i + f.param_index].type];
    return os;
}

std::ostream& operator<<(std::ostream& os, SymbolTable& st) {
    for (size_t i = 0; i < st.symbol_list.size(); i++) {
        os << i << "#" << st.symbol_list[i];
        auto& s = st.symbol_list[i];
        if (oneof(s.cat, Cat_Var, Cat_Param)) {
            os << ":" << ST.type_list[s.type];
            os << "&" << s.offset;
        }
        if (oneof(s.cat, Cat_Func_Declaration, Cat_Func_Defination, Cat_Const, Cat_Temp)) {
            os << ":" << ST.type_list[s.type];
        }
        if (s.cat == Cat_Type) {
            os << ":" << ST.type_list[s.type];
            os << "*" << ST.type_list[s.type].size;
        }
        os << endl;
    }
    return os;
}

std::string SymbolTable::get_symbol_name(size_t symbol) {
    return ST[symbol].name;
}

bool SymbolTable::is_temp_var(size_t symbol) {
    return ST[symbol].cat == Cat_Param ||
        (ST[symbol].name[0] == '@' && isdigit(ST[symbol].name[1]));
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
    return function_list[type_list[symbol_list[symbol].type].data].param_num;
}

int SymbolTable::get_func_param_size(size_t symbol) {
    int a = 0;
    for (size_t i = 0; i < function_list[type_list[symbol_list[symbol].type].data].param_num; i++) {
        a += type_list[symbol_list[function_list[type_list[symbol_list[symbol].type].data].param_index + i].type].size;
    }
    return a;
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

void SymbolTable::set_symbol_offset(size_t symbol, int off) {
    symbol_list[symbol].offset = off;
}

size_t SymbolTable::get_or_add_label(const std::string &name) {
    int i = get_symbol_index_by_name_without_error(name);
    if (i == -1)
        return add_symbol({name, 0, Cat_Label});
    else if (symbol_list[i].cat == Cat_Label)
        return static_cast<size_t>(i);
    error("label " + name + " is already defined.");
    return 0;
}

SymbolTable::Type &SymbolTable::get_type_by_index(size_t t) {
    return type_list[t];
}

std::string SymbolTable::get_top_type_name(size_t symbol) {
    switch (TYPE(symbol).t) {
        case INT:
            return "int";
        case SHORT:
            return "short";
        case CHAR:
            return "char";
        case LONG:
            return "long";
        case FLOAT:
            return "float";
        case DOUBLE:
            return "double";
        case FUNCTION:
            return "function";
        case ARRAY:
            return "array";
        case STRUCT:
            return "struct";
        case POINTER:
            return "pointer";
    }
    rterr("unsupported type");
}

SymbolTable::Function &SymbolTable::get_function_by_index(size_t i) {
    return function_list[i];
}

SymbolTable::Function& SymbolTable::get_function_by_symbol(size_t symbol) {
    return function_list[TYPE(symbol).data];
}

bool SymbolTable::is_define_var(size_t symbol) {
    return oneof(ST[symbol].cat, Cat_Var, Cat_Param) && !is_temp_var(symbol);
}

bool SymbolTable::is_const(size_t symbol) {
    return type_list[symbol_list[symbol].type].cst;
}





