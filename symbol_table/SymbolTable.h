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
#include "../word_analysis/TokenList.h"


typedef Token Item;

#define NEW_S(x) (new Item{'y', (x)})
#define NEW_T(x) (new Item{'t', (x)})

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

#define ARRAY 0x0B
#define FUNCTION 0x0C
#define STRUCT 0x0D
#define UNION 0x0E
#define ENUM 0x0F




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

    // 必须保证Type添加到set后不改变
    struct Type {
        int t;
        size_t size;
        size_t data;

        bool operator==(const Type& an) const {
            return t == an.t && data == an.data;
        }
        bool operator<(const Type& an) const {
            return t + data < an.t + an.data;
        }
    };


    struct Symbol {
        std::string name;
        size_t type;
        int cat = 0;
        int offset;
        // size_t data;
    };


    struct Array {
        size_t len;
        size_t type;
    };

    struct Struct {
        size_t num_fields;
        size_t data;
    };

    struct Function {
        uint param_num;
        size_t ret_type;
        size_t param_index;
        uint entry;
    };

    struct Label {
        uint offset;
    };

    struct Table {
        std::map<std::string, size_t> symbol_index;
        std::map<Type, size_t> type_index;

        Table* up = nullptr;
    };

private:
    Table* top_table;
    Table* current_table;
    int temp_cnt = 0;

    std::vector<SymbolTable::Symbol> symbol_list;
    std::vector<Type> type_list;
    std::vector<Struct> struct_list;
    std::vector<SymbolTable::Array> array_list;
    std::vector<SymbolTable::Function> function_list;

    std::vector<Number> constant_num_list;
    std::vector<Label> label_list;
public:

    size_t get_or_add_type(const Type& type);
    const Symbol* get_symbol_by_name(const std::string& name);

    size_t add_symbol(const Symbol& s);
    size_t add_constant_Symbol(const Number& num);

    Symbol& operator[](size_t i);

    int get_symbol_index_by_name(const std::string& name);
    size_t get_type_index_by_symbol(size_t symbol);
    size_t get_struct_field_by_symbol_filed(size_t s, size_t f);
    size_t get_array_element_type(size_t symbol);
    size_t get_function_type(size_t symbol);
    size_t get_type_size(size_t symbol);

    Type& get_type_by_symbol(size_t i);

    std::string get_temp_var_name(const std::string& suffix="");
    void* add_veriables(void* tv, void* vv);

    void in();
    void leave();

};

struct TypeBuilder {
    static void* add_speicifer(void* it, void* t=nullptr);
    static void* add_qulifier(size_t key_in_token, void* t=nullptr);
    static void* add_storage(size_t key_in_token, void* t=nullptr);
};



#endif //COMPLIE_SYMBOLTABLE_H
