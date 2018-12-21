//
// Created by wmj on 18-11-29.
//

#ifndef COMPLIE_SYMBOLTABLE_H
#define COMPLIE_SYMBOLTABLE_H

#include <utility>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <atomic>

#include "../word_analysis/NumericDFA.h"
#include "../word_analysis/TokenList.h"


typedef Token Item;

#define NEW_S(x) (new Item{'y', (x)})
#define NEW_T(x) (new Item{'t', (x)})
#define TEMP_S SymbolTable::TempSymbol

#define ST SymbolTable::getInstance()

#define Cat_Var 0
#define Cat_Param 1
#define Cat_Field 2
#define Cat_Arr_ele 3
#define Cat_Stru_ele 4
#define Cat_Label 5
#define Cat_Temp 6
#define Cat_Const 7
#define Cat_Type 8
#define Cat_Func_Declaration 9
#define Cat_Func_Defination 10

#define CONST 0x40
#define UNSIGNED 0x20
#define POINTER 0x10

#define CHAR 0x01
#define SHORT 0x02
#define INT 0x03
#define LONG 0x04
#define FLOAT 0x05
#define DOUBLE 0x06
#define VOID 0x07

#define ARRAY 0x0B
#define FUNCTION 0x0C
#define STRUCT 0x0D
#define UNION 0x0E
#define ENUM 0x0F

#define insertaddr(a, b) ST.set_symbol_addr((a), (b))
#define getaddr(a) ST.get_symbol_addr(a)
#define getoffset(a) ST.get_symbol_offset(a)
#define getparamnum(a) ST.get_func_param_size(a)
#define getvalue(a) ST.get_basic_symbol_type(a)
#define isterm(a) ST.is_temp_var(a)
#define getname(a) ST.get_symbol_name(a)


class SymbolTable {
private:
    SymbolTable();
public:
    SymbolTable(SymbolTable& ano) = delete;
    SymbolTable& operator=(SymbolTable& an) = delete;
    ~SymbolTable();

    static SymbolTable& getInstance()
    {
        static SymbolTable instance;
        return instance;
    }

    // 必须保证Type添加到set后不改变
    struct Type {
        int t;
        size_t size;
        size_t data;
        bool cst;

        Type(int t, size_t size=0, size_t data=0, bool cst=false): t(t), size(size), data(data), cst(cst) {}
        bool operator==(const Type& an) const {
            return t == an.t && data == an.data && cst == an.cst;
        }
        bool operator<(const Type& an) const {
            return ((t<<16) | (data<<8) | cst) < ((an.t<<16) | (an.data<<8) | an.cst);
        }
    };


    struct Symbol {
        std::string name;
        size_t type;
        int cat = 0;
        int offset;
        size_t addr=0;
        bool is_addr = false;

        Symbol(std::string name, size_t type, int cat, int offset=0): name(std::move(name)), type(type), cat(cat), offset(offset) {}
    };


    struct Array {
        size_t len;
        size_t type;
        Array(size_t a, size_t b): len(a), type(b) {}
        bool operator<(const Array& r) const {
            return len + type < r.len + r.type;
        }
        bool operator==(const Array& r) const {
            return len == r.len && type == r.type;
        }
    };

    struct Struct {
        size_t num_fields;
        size_t data;

        Struct(size_t n, size_t d): num_fields(n), data(d) {}
    };

    struct Function {
        uint param_num;
        size_t ret_type;
        size_t param_index;
        uint entry;

        Function(uint param_num, size_t ret_type, size_t param_index, uint entry):
                param_num(param_num), ret_type(ret_type), param_index(param_index), entry(entry) {}
    };

    struct Label {
        uint offset;

        explicit Label(uint off): offset(off) {}
    };

    struct Table {
        std::map<std::string, size_t> symbol_index;
        std::map<Type, size_t> type_index;

        Table* up = nullptr;
        int offset = 0;
        bool next_func = false;
    };

private:
    Table* top_table;
    Table* current_table;
    int temp_cnt = 0;

    std::vector<SymbolTable::Symbol> symbol_list;
    std::vector<Type> type_list;
    std::vector<Struct> struct_list;
    std::vector<SymbolTable::Array> array_list;
    std::map<SymbolTable::Array, size_t> array_map;
    std::vector<SymbolTable::Function> function_list;
    std::map<SymbolTable::Function, size_t> function_map;

    std::vector<Number> constant_num_list;
    std::vector<Label> label_list;
    std::vector<std::string> asm_list;

    size_t recursive_type(size_t type);

public:
    size_t add_asm(const std::string& s);
    std::string get_asm(size_t i);

    size_t get_or_add_type(const Type& type);
    size_t get_or_add_array(const Array& array);
    size_t get_or_add_function(size_t return_type, const std::vector<size_t>& param_type);
    const Symbol* get_symbol_by_name(const std::string& name);

    size_t add_symbol(const Symbol& s);
    size_t get_or_add_constant(const Number &num);
    size_t get_or_add_label(const std::string& name);

    Symbol& operator[](size_t i);

    size_t get_symbol_index_by_name(const std::string& name);
    int get_symbol_index_by_name_without_error(const std::string& name);
    size_t get_type_index_by_symbol(size_t symbol);
    size_t get_struct_field_by_symbol_filed(size_t s, size_t f);
    size_t get_array_element_type(size_t symbol);
    size_t get_array_length(size_t symbol);
    Function& get_function_by_symbol(size_t symbol);
    Struct& get_struct_by_symbol(size_t symbol);
    size_t get_function_type(size_t symbol);
    size_t get_type_size(size_t symbol);

    size_t get_basic_symbol_type(size_t symbol);

    std::string get_symbol_name(size_t symbol);
    bool is_temp_var(size_t symbol);
    bool is_define_var(size_t symbol);
    void set_symbol_addr(size_t symbol, size_t addr);
    void set_symbol_offset(size_t symbol, int off);
    size_t get_symbol_addr(size_t symbol);
    int get_symbol_offset(size_t symbol);
    int get_func_param_num(size_t symbol);
    int get_func_param_size(size_t symbol);
    std::string get_type_name(size_t symbol);  // 数组返回基本类型
    std::string get_top_type_name(size_t symbol);
    bool is_const(size_t symbol);
    bool is_array(size_t symbol);

    Type& get_type_by_symbol(size_t i);
    Type& get_type_by_index(size_t);
    Function& get_function_by_index(size_t i);

    std::string get_temp_var_name(const std::string& suffix="");

    // param: TypeBuilder*, vector<size_t>* v;
    size_t set_variables_type(void* tv, void* vv, int cat=Cat_Var);

    size_t add_struct_or_union(size_t struct_or_union, size_t* declaration_list, size_t* pos);
    size_t add_struct_or_union(size_t struct_or_union, void* name, size_t* declaration_list, size_t* pos);

    bool is_symbol_exists(const std::string& name);

    struct TempSymbol {
        SymbolTable::Symbol s;
        std::vector<SymbolTable::Type> tl;
        std::vector<SymbolTable::Array> al;
        std::vector<SymbolTable::Function> fl;
        std::vector<SymbolTable::TempSymbol*> fpl;

        std::vector<size_t> initializer_list;

        size_t insert_symbol_into_table(int cat);

        explicit TempSymbol(const std::string& s): s(s, 0, Cat_Var, 0) {
            // std::cout << "temp symbol " << s << std::endl;
            tl.emplace_back(0, 0, 0);
        }
        TempSymbol(): s("@anonymous", 0, Cat_Var, 0) {
            // std::cout << "temp symbol " << s << std::endl;
            tl.emplace_back(0, 0, 0);
        }

        TempSymbol* add_array(size_t len);
        TempSymbol* add_array();
        TempSymbol* add_function(std::vector<SymbolTable::TempSymbol*>& v);
        TempSymbol* add_basic_type(SymbolTable::Type& type);
        TempSymbol* set_initializer_list(std::vector<size_t>& list);
        static size_t add_basic_type_and_insert_into_table(std::vector<TempSymbol*> v, SymbolTable::Type* t, int cat);
        static size_t add_basic_type_and_insert_into_table(TempSymbol* v, SymbolTable::Type* t, int cat);

        TempSymbol* merge_pointer(std::vector<SymbolTable::Type>* pointer_ype_list);

        int first_type_t();
    private:
        size_t insert_type_into_table(size_t ti);
        size_t insert_array_into_table(size_t ai);
        size_t insert_function_into_table(size_t fi);
    };

    void in();
    void in(std::map<std::string, size_t> symbol_index);
    void leave();


    friend std::ostream& operator<<(std::ostream& os, Type& t);
    friend std::ostream& operator<<(std::ostream& os, Symbol& s);
    friend std::ostream& operator<<(std::ostream& os, Array& a);
    friend std::ostream& operator<<(std::ostream& os, SymbolTable& st);
    friend std::ostream& operator<<(std::ostream& os, SymbolTable::Function& f);
};

struct TypeBuilder {
    static void* add_speicifer(void* it, void* t=nullptr);
    static void* add_qulifier(size_t key_in_token, void* t=nullptr);
    static void* add_storage(size_t key_in_token, void* t=nullptr);
};

using FunctionParamTemp = std::vector<SymbolTable::TempSymbol*>;

std::ostream& operator<<(std::ostream& os, SymbolTable::Symbol& s);
std::ostream& operator<<(std::ostream& os, SymbolTable::Array& a);
std::ostream& operator<<(std::ostream& os, SymbolTable::Type& t);
std::ostream& operator<<(std::ostream& os, SymbolTable& st);
std::ostream& operator<<(std::ostream& os, SymbolTable::Function& f);




#endif //COMPLIE_SYMBOLTABLE_H
