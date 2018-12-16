//
// Created by wmj on 18-11-27.
//

#include "Grammar.h"
#include "../Utility.h"
#include "Attribute.h"
#include "../symbol_table/SymbolTable.h"
#include "../symbol_table/Quaternary.h"
#include "../word_analysis/NumericDFA.h"
#include "../word_analysis/WordAnalysis.h"


using namespace std;

Generators Grammar::Basic_Exp() {
    Generators gen;

    gen.set_terminators({"+", "-", "*", "/", "(", ")", "i"});
    gen.set_non_terminators({"E", "T", "F"});
    gen.set_start("E");
    gen.add("E") >> "E + T" | "E - T" | "T";
    gen.add("T") >> "T * F" | "T / F" | "F";
    gen.add("F") >> "( E )" | "i";

    return gen;
}

Generators Grammar::C_Bin_Exp() {
    Generators gen;

    gen.set_terminators("= += -= *= /= %= <<= >>= &= ^= |= ? : "
                        "|| && | ^ & == != <= >= > < << >> + - *"
                        " / % ++ -- ! ~ ( ) sizeof [ ] . -> i t ,");
    gen.set_non_terminators("Y Z A B C D E F G H I J K L M N O");
    gen.set_start("Z");
    gen.add("Z") >> "Z , A" | "A";
    gen.add("A") >> "B";
//    gen.add("A") >> "B = A" | "B += A" | "B -= A" | "B *= A" | "B /= A" | "B %= A" |
//                    "B <<= A" | "B >>= A" | "B &= A" | "B ^= A" | "B |= A" | "B";
    gen.add("B") >> "B ? B : C" | "C";
    gen.add("C") >> "C || D" | "D";
    gen.add("D") >> "D && E" | "E";
    gen.add("E") >> "E | F" | "F";
    gen.add("F") >> "F ^ G" | "G";
    gen.add("G") >> "G & H" | "H";
    gen.add("H") >> "H == I" | "H != I" | "I";
    gen.add("I") >> "I <= J" | "I >= J" | "J";
    gen.add("J") >> "J << K" | "J >> K" | "K";
    gen.add("K") >> "K + L" | "K - L" | "L";
    gen.add("L") >> "L * M" | "L / M" | "L % M" | "M";
//    gen.add("M") >> "N";
    gen.add("M") >> "++ M" | "-- M" | "+ M" | "- M" | "! M" | "~ M" | "( t ) M" |
    "* M" | "& M" | "sizeof ( t )" | "N";
    gen.add("N") >> "O";
//    gen.add("N") >> "O ++" | "O --" | "O ( Y )" | "O [ Z ]" | "O . i" | "O -> i" | "O";
    gen.add("O") >> "( Z )" | "i";
    //gen.add("Y") >> "Z , Y" | "Z";

    return gen;
}

Generators Grammar::FG_C_Grammar() {
    Generators gen;

    gen.set_terminators("int_const float_const id ; , > < >= <= == != + - * / && || . ! = "
                        "int float ( ) [ ] { } struct return if else while");
    gen.set_non_terminators("SEMI COMMA ASSIGNOP RELOP MINUS PLUS STAR DIV AND OR NOT STRUCT RETURN "
                            "DOT TYPE LP RP LB RB LC RC struct return if else while Program ExtDefList ExtDef IF ELSE WHILE CompSt Tag "
                            "IDENTIFIER INT StmtList Stmt Exp Args "
                            "ExtDecList Specifier StructSpecifier OptTag VarDec FunDec VarList ParamDec DefList Def DecList Dec");
    gen.set_start("Program");

    // TOKENS
    gen << "SEMI" >> ";";
    gen << "COMMA" >> ",";
    gen << "ASSIGNOP" >> "=";
    gen << "RELOP" >> ">" | "<" | ">=" | "<=" | "==" | "!=";
    gen << "PLUS" >> "+";
    gen << "MINUS" >> "-";
    gen << "STAR" >> "*";
    gen << "DIV" >> "/";
    gen << "AND" >> "&&";
    gen << "OR" >> "||";
    gen << "DOT" >> ".";
    gen << "NOT" >> "!";
    gen << "TYPE" >> "int" | "float";
    gen << "LP" >> "(";
    gen << "RP" >> ")";
    gen << "LB" >> "[";
    gen << "RB" >> "]";
    gen << "LC" >> "{";
    gen << "RC" >> "}";
    gen << "STRUCT" >> "struct";
    gen << "RETURN" >> "return";
    gen << "IF" >> "if";
    gen << "ELSE" >> "else";
    gen << "WHILE" >> "while";

    // High-level Definitions
    gen << "Program" >> "ExtDefList";
    gen << "ExtDefList" >> "ExtDef ExtDefList" | gen.get_epsilon();
    gen << "ExtDef" >> "Specifier ExtDecList SEMI"
                       | "Specifier SEMI"
                       | "Specifier FunDec CompSt";

    gen << "ExtDecList" >> "VarDec"
                       | "VarDec COMMA ExtDecList";

    // Sepcifiers
    gen << "Specifier" >> "TYPE" | "StructSpecifier";
    gen << "StructSpecifier" >> "STRUCT OptTag LC DefList RC" | "STRUCT Tag";
    gen << "OptTag" >> "IDENTIFIER" | gen.get_epsilon();
    gen << "Tag" >> "IDENTIFIER";

    //Declarators
    gen << "VarDec" >> "IDENTIFIER" | "VarDec LB INT RB";
    gen << "FunDec" >> "IDENTIFIER LP VarList RP" | "IDENTIFIER LP RP";
    gen << "VarList" >> "ParamDec COMMA VarList" | "ParamDec";
    gen << "ParamDec" >> "Specifier VarDec";

    // Statements
    gen << "CompSt" >> "LC DefList StmtList RC";
    gen << "StmtList" >> "Stmt StmtList"| gen.get_epsilon();
    gen << "Stmt" >> "Exp SEMI"|
            "CompSt"| "RETURN Exp SEMI"|
            "IF LP Exp RP Stmt"|
            "IF LP Exp RP Stmt ELSE Stmt"|
            "WHILE LP Exp RP Stmt";

    // Local Definitions
    gen << "DefList" >> "Def DefList" | gen.get_epsilon();
    gen << "Def" >> "Specifier DecList SEMI";
    gen << "DecList" >> "Dec" | "Dec COMMA DecList";
    gen << "Dec" >> "VarDec" | "VarDec ASSIGNOP Exp";

    // Expressions
    gen << "Exp" >> "Exp PLUS Exp";
    gen << "Args" >> "Exp COMMA Args"| "Exp";

    return gen;
}

Generators Grammar::Basic_Exp_Quat(TokenList &tokenList) {
    Generators gen;

//    Attribute *attr = new Attr_ExpQuat_2_LR(tokenList);
//    gen.set_attr_builder(attr);
//
//    gen.set_terminators({"+", "-", "*", "/", "(", ")", "i"});
//    gen.set_non_terminators({"E", "T", "F"});
//    gen.set_start("E");
//
//    gen.add("E") >> "E + T" | attr |
//    "E - T" | attr |
//    "T";
//    gen.add("T") >> "T * F" | attr |
//    "T / F" | attr |
//    "F";
//    gen.add("F") >> "( E )" | new process_B |
//    "i";

    return gen;
}

Generators Grammar::C_Exp() {
    Generators gen;

    gen.set_terminators("= += -= *= /= %= <<= >>= &= ^= |= ? : "
                        "|| && | ^ & == != <= >= > < << >> + - *"
                        " / % ++ -- ! ~ ( ) sizeof [ ] . -> i t c s ,");
    gen.set_non_terminators("Exp AssignExp ConditionalExp UnaryExp AssignOperator BinaryExp PostfixExp UnaryExp "
                            "PostfixOperator PriExp BinaryOperator UnaryOperator Exp_A Cond_A Bin_A Post_A PostOP_A PostOP_B");

    gen.set_start("Exp");

    gen << "Exp" >> "AssignExp Exp_A";
    gen << "Exp_A" >> ", AssignExp Exp_A" | gen.get_epsilon();

    gen << "AssignExp" >> "ConditionalExp" |
    "UnaryExp AssignOperator AssignExp";
    gen << "AssignOperator" >> "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "<<=" | ">>=" | "&=" | "^=" | "|=";

    gen << "ConditionalExp" >> "BinaryExp Cond_A";
    gen << "Cond_A" >> "? Exp : ConditionalExp" | gen.get_epsilon();

    gen << "BinaryExp" >> "UnaryExp Bin_A";
    gen << "Bin_A" >> "BinaryOperator UnaryExp Bin_A" | gen.get_epsilon();

    gen << "BinaryOperator" >> "||" | "&&" | "|" | "^" | "&" | "==" | "!=" | "<" | ">" | "<=" | ">=" | "<<" | ">>" |
    "+" | "-" | "*" | "/" | "%";

    gen << "UnaryExp" >> "PostfixExp" | "UnaryOperator UnaryExp" | "( t ) UnaryExp" | "sizeof UnaryExp" |
    "sizeof ( t )";

    gen << "UnaryOperator" >> "++" | "--" | "&" | "*" | "+" | "-" | "~" | "!";

    gen << "PostfixExp" >> "PriExp Post_A";
    gen << "Post_A" >> "PostfixOperator Post_A" | gen.get_epsilon();

    gen << "PostfixOperator" >> "[ Exp ]" | "( PostOP_A )" | ". i" | "-> i" | "++" | "--";
    gen << "PostOP_A" >> "AssignExp PostOP_B" | gen.get_epsilon();
    gen << "PostOP_B" >> ", AssignExp PostOP_B" | gen.get_epsilon();

    gen << "PriExp" >> "i" | "c" | "s" | "( Exp )";

    return gen;
}

Generators Grammar::YACC_C_Grammar() {
    Generators gen;
    gen.set_terminators(
            "IDENTIFIER CONSTANT STRING_LITERAL sizeof "
            "-> ++ -- << >> <= >= == != "
            "&& || *= /= %= += "
            "-= <<= >>= &= "
            "^= |= TYPE_NAME "
            "typedef extern static auto register "
            "char short int long signed unsigned float double const volatile void "
            "struct union enum ... "
            "case default if else switch while do for goto continue break return "
            "( ) [ ] { } . -> , & | ^ + - * / ~ % ! < > ? : = ;"
    );

    gen.set_non_terminators(
            "primary_expression postfix_expression argument_expression_list unary_expression unary_operator "
            "cast_expression multiplicative_expression additive_expression shift_expression relational_expression "
            "equality_expression and_expression exclusive_or_expression inclusive_or_expression logical_and_expression "
            "logical_or_expression conditional_expression assignment_expression assignment_operator expression "
            "constant_expression declaration declaration_specifiers init_declarator_list init_declarator "
            "storage_class_specifier type_specifier struct_or_union_specifier struct_or_union struct_declaration_list "
            "struct_declaration specifier_qualifier_list struct_declarator_list struct_declarator "
            "enum_specifier enumerator_list enumerator type_qualifier declarator direct_declarator pointer "
            "type_qualifier_list parameter_type_list parameter_list parameter_declaration identifier_list "
            "type_name abstract_declarator direct_abstract_declarator initializer initializer_list statement "
            "labeled_statement compound_statement declaration_list statement_list expression_statement "
            "selection_statement iteration_statement jump_statement translation_unit external_declaration "
            "function_definition");

    gen.set_start("translation_unit");

    gen.set_attr_builder(default_builder);


    gen.add("primary_expression")
    | "IDENTIFIER"| ATTR{ return NEW_S(ST.get_symbol_index_by_name(*((string*)v[0])));}
    | "CONSTANT"| ATTR{ return NEW_S(ST.add_constant_Symbol(SL.get_number(ITEM_V(0))));}
    | "STRING_LITERAL"
    | "( expression )" | ATTR{return v[1];}
            ;

    gen.add("postfix_expression")
    | "primary_expression"
    | "postfix_expression [ expression ]"
    | "postfix_expression ( )" | ATTR{return NEW_S(quat(OP::CALL, ITEM_V(0), NONE));}
    | "postfix_expression ( argument_expression_list )" | ATTR{return NEW_S(quat(OP::CALL, ITEM_V(0), NONE));}
    | "postfix_expression . IDENTIFIER"
    | "postfix_expression -> IDENTIFIER"
    | "postfix_expression ++" | ATTR{quat(OP::INC, ITEM_V(0), NONE, ITEM_V(0)); return v[0];}  // TODO: 区分前后++
    | "postfix_expression --" | ATTR{quat(OP::DEC, ITEM_V(0), NONE, ITEM_V(0)); return v[0];}
            ;

    // 调用函数时的参数列表
    gen.add("argument_expression_list")
    | "assignment_expression" | ATTR{quat(OP::PUSH, ITEM_V(0), NONE); return nullptr;}
    | "argument_expression_list , assignment_expression"| ATTR{quat(OP::PUSH, ITEM_V(2), NONE); return nullptr;}
            ;

    gen.add("unary_expression")
    | "postfix_expression"
    | "++ unary_expression" | ATTR{quat(OP::INC, ITEM_V(1), NONE, ITEM_V(0)); return v[1];}
    | "-- unary_expression" | ATTR{quat(OP::DEC, ITEM_V(1), NONE, ITEM_V(0)); return v[1];}
    | "unary_operator cast_expression" | ATTR{return NEW_S(make_unary_operator_quat(v[0], ITEM_V(1)));}
    | "sizeof unary_expression" | ATTR{return NEW_S(ST.add_constant_Symbol({Number::ULong, ST.get_type_size(ITEM_V(1))}));}
    | "sizeof ( type_name )" | ATTR{return NEW_S(ST.add_constant_Symbol({Number::ULong, ST.get_type_size(ITEM_V(1))}));}
            ;

    gen.add("unary_operator")
    | "&"
    | "*"
    | "+"
    | "-"
    | "~"
    | "!"
            ;

    gen.add("cast_expression")
    | "unary_expression"
    | "( type_name ) cast_expression"| ATTR{ST[ITEM_V(3)].type = ST[ITEM_V(1)].type; return v[3];}
            ;

    gen.add("multiplicative_expression")
    | "cast_expression"
    | "multiplicative_expression * cast_expression"| ATTR{return NEW_S(quat(OP::MULTPLY, ITEM_V(0), ITEM_V(2)));}
    | "multiplicative_expression / cast_expression"| ATTR{return NEW_S(quat(OP::DIVIDE, ITEM_V(0), ITEM_V(2)));}
    | "multiplicative_expression % cast_expression"| ATTR{return NEW_S(quat(OP::MOD, ITEM_V(0), ITEM_V(2)));}
            ;

    gen.add("additive_expression")
    | "multiplicative_expression"
    | "additive_expression + multiplicative_expression"| ATTR{return NEW_S(quat(OP::PLUS, ITEM_V(0), ITEM_V(2)));}
    | "additive_expression - multiplicative_expression"| ATTR{return NEW_S(quat(OP::MINUS, ITEM_V(0), ITEM_V(2)));}
            ;

    gen.add("shift_expression")
    | "additive_expression"
    | "shift_expression << additive_expression"| ATTR{return NEW_S(quat(OP::SHIFT_LEFT, ITEM_V(0), ITEM_V(2)));}
    | "shift_expression >> additive_expression"| ATTR{return NEW_S(quat(OP::SHIFT_RIGHT, ITEM_V(0), ITEM_V(2)));}
            ;

    gen.add("relational_expression")
    | "shift_expression"
    | "relational_expression < shift_expression"| ATTR{return NEW_S(quat(OP::LESS_THAN, ITEM_V(0), ITEM_V(2)));}
    | "relational_expression > shift_expression"| ATTR{return NEW_S(quat(OP::GREATER_THEN, ITEM_V(0), ITEM_V(2)));}
    | "relational_expression <= shift_expression"| ATTR{return NEW_S(quat(OP::LESS_EQUAL, ITEM_V(0), ITEM_V(2)));}
    | "relational_expression >= shift_expression"| ATTR{return NEW_S(quat(OP::GREATER_EQUAL, ITEM_V(0), ITEM_V(2)));}
            ;

    gen.add("equality_expression")
    | "relational_expression"
    | "equality_expression == relational_expression"| ATTR{return NEW_S(quat(OP::EQUAL, ITEM_V(0), ITEM_V(2)));}
    | "equality_expression != relational_expression"| ATTR{return NEW_S(quat(OP::NOT_EQUAL, ITEM_V(0), ITEM_V(2)));}
            ;

    gen.add("and_expression")
    | "equality_expression"
    | "and_expression & equality_expression"| ATTR{return NEW_S(quat(OP::BIT_AND, ITEM_V(0), ITEM_V(2)));}
            ;

    gen.add("exclusive_or_expression")
    | "and_expression"
    | "exclusive_or_expression ^ and_expression"| ATTR{return NEW_S(quat(OP::BIT_XOR, ITEM_V(0), ITEM_V(2)));}
            ;

    gen.add("inclusive_or_expression")
    | "exclusive_or_expression"
    | "inclusive_or_expression | exclusive_or_expression"| ATTR{return NEW_S(quat(OP::BIT_OR, ITEM_V(0), ITEM_V(2)));}
            ;

    gen.add("logical_and_expression")
    | "inclusive_or_expression"
    | "logical_and_expression && inclusive_or_expression"| ATTR{return NEW_S(quat(OP::LOG_AND, ITEM_V(0), ITEM_V(2)));}
            ;

    gen.add("logical_or_expression")
    | "logical_and_expression"
    | "logical_or_expression || logical_and_expression"| ATTR{return NEW_S(quat(OP::LOG_OR, ITEM_V(0), ITEM_V(2)));}
            ;

    gen.add("conditional_expression")
    | "logical_or_expression"
    | "logical_or_expression ? expression : conditional_expression"
            ;

    gen.add("assignment_expression")
    | "conditional_expression"
    | "unary_expression assignment_operator assignment_expression"|
        ATTR{return NEW_S(make_assign_operator_quat(ITEM_V(0), ITEM_V(1), ITEM_V(2)));}
            ;

    gen.add("assignment_operator")
    | "="
    | "*="
    | "/="
    | "%="
    | "+="
    | "-="
    | "<<="
    | ">>="
    | "&="
    | "^="
    | "|="
            ;

    gen.add("expression")
    | "assignment_expression" | ATTR{ attr_stmt_pos(); return v[0]; }
    | "expression , assignment_expression"| ATTR{attr_stmt_pos(); return v[2];} // 逗号运算符？
            ;

    gen.add("constant_expression")
    | "conditional_expression"
            ;

    gen.add("declaration")
    | "declaration_specifiers ;"| ATTR{ return attr_stmt_pos(); }
    | "declaration_specifiers init_declarator_list ;" | ATTR{ST.add_veriables(v[0], v[1]); return attr_stmt_pos();}
            ;

    // 类型名: [const|validate] [typedef|extern|static|auto|register] [int|float|...]
    // TODO: 任何类型标志都只能出现一次, sturct定义也一样
    gen.add("declaration_specifiers")
    | "storage_class_specifier"| ATTR{return TypeBuilder::add_storage(ITEM_V(0));}
    | "storage_class_specifier declaration_specifiers" | ATTR{return TypeBuilder::add_storage(ITEM_V(0), v[1]);}
    | "type_specifier"| ATTR{return TypeBuilder::add_speicifer(v[0]); }
    | "type_specifier declaration_specifiers"| ATTR{return TypeBuilder::add_speicifer(v[0], v[1]); }
    | "type_qualifier"| ATTR{return TypeBuilder::add_qulifier(ITEM_V(0));}
    | "type_qualifier declaration_specifiers"| ATTR{return TypeBuilder::add_qulifier(ITEM_V(0), v[1]);}
            ;

    gen.add("init_declarator_list")
    | "init_declarator" | ATTR{ return new vector<size_t>{ITEM_V(0)};}
    | "init_declarator_list , init_declarator"| ATTR{((vector<size_t>*)v[0])->push_back(ITEM_V(2)); return v[0]; }
            ;

    // TODO: 检测初始化类型匹配
    gen.add("init_declarator")
    | "declarator"| ATTR{return NEW_S(((SymbolTable::TempSymbol*)v[0])->insert_symbol_into_table(Cat_Var));}
    | "declarator = initializer"| ATTR{return NEW_S(quat(OP::ASSIGN, ITEM_V(2), NONE,
                                                         ((SymbolTable::TempSymbol*)v[0])->insert_symbol_into_table(Cat_Var)));}
            ;

    gen.add("storage_class_specifier")
    | "typedef"
    | "extern"
    | "static"
    | "auto"
    | "register"
            ;

    gen.add("type_specifier")
    | "void"
    | "char"
    | "short"
    | "int"
    | "long"
    | "float"
    | "double"
    | "signed"
    | "unsigned"
    | "struct_or_union_specifier"
    | "enum_specifier"
    | "TYPE_NAME"
            ;

    gen.add("struct_or_union_specifier")
    | "struct_or_union IDENTIFIER { struct_declaration_list }" | ATTR{return NEW_S(ST.add_struct_or_union(ITEM_V(0),v[1], ITEM_V(3)));}
    | "struct_or_union { struct_declaration_list }" | ATTR{return NEW_S(ST.add_struct_or_union(ITEM_V(0), ITEM_V(2)));}
    | "struct_or_union IDENTIFIER" | ATTR {return NEW_S(ST.add_symbol({*((string*)v[1]), ST.get_or_add_type({STRUCT, 0, 0}), Cat_Var}));}
            ;

    gen.add("struct_or_union")
    | "struct"
    | "union"
            ;

    gen.add("struct_declaration_list")
    | "struct_declaration"
    | "struct_declaration_list struct_declaration" | ATTR{return v[0]; }  // 始终指向第一个成员
            ;

    gen.add("struct_declaration")
    | "specifier_qualifier_list struct_declarator_list ;"| ATTR{ return NEW_S(ST.add_veriables(v[0], v[1], Cat_Stru_ele)); }
            ;

    gen.add("specifier_qualifier_list")
    | "type_specifier specifier_qualifier_list"| ATTR{return TypeBuilder::add_speicifer(v[0], v[1]); }
    | "type_specifier"| ATTR{return TypeBuilder::add_speicifer(v[0]); }
    | "type_qualifier specifier_qualifier_list"| ATTR{return TypeBuilder::add_qulifier(ITEM_V(0), v[1]); }
    | "type_qualifier"| ATTR{return TypeBuilder::add_qulifier(ITEM_V(0));}
            ;

    gen.add("struct_declarator_list")
    | "struct_declarator"|  ATTR{ return new vector<size_t>{ITEM_V(0)};}
    | "struct_declarator_list , struct_declarator"| ATTR{((vector<size_t>*)v[0])->push_back(ITEM_V(2)); return v[0]; }
            ;

    gen.add("struct_declarator")
    | "declarator" | ATTR{return NEW_S(((SymbolTable::TempSymbol*)v[0])->insert_symbol_into_table(Cat_Stru_ele));}
    | ": constant_expression"
    | "declarator : constant_expression"
            ;

    gen.add("enum_specifier")
    | "enum { enumerator_list }"
    | "enum IDENTIFIER { enumerator_list }"
    | "enum IDENTIFIER"
            ;

    gen.add("enumerator_list")
    | "enumerator"
    | "enumerator_list , enumerator"
            ;

    gen.add("enumerator")
    | "IDENTIFIER"
    | "IDENTIFIER = constant_expression"
            ;

    gen.add("type_qualifier")
    | "const"
    | "volatile"
            ;

    gen.add("declarator")
    | "pointer direct_declarator"| ATTR{ return ((SymbolTable::TempSymbol*)v[1])->merge_pointer((vector<SymbolTable::Type>*)v[0]); }
    | "direct_declarator"
            ;

    gen.add("direct_declarator")
    | "IDENTIFIER" | ATTR{ return new SymbolTable::TempSymbol(*((string*) v[0]));}
    | "( declarator )"
    | "direct_declarator [ constant_expression ]" | ATTR{return ((SymbolTable::TempSymbol*)v[0]) -> add_array(ITEM_V(2));}
    | "direct_declarator [ ]"| ATTR{return ((SymbolTable::TempSymbol*)v[0]) -> add_array();}
    | "direct_declarator ( parameter_type_list )"
    | "direct_declarator ( identifier_list )"
    | "direct_declarator ( )"
            ;

    gen.add("pointer")
    | "*"| ATTR{ return new vector<SymbolTable::Type>{POINTER}; }
    | "* type_qualifier_list" | ATTR{ return new std::vector<SymbolTable::Type>{0, 0, 0, true}; }  // 不支持volatile
    | "* pointer"| ATTR{((vector<SymbolTable::Type>*)v[1])->push_back({POINTER}); return v[1];}
    | "* type_qualifier_list pointer"| ATTR{((vector<SymbolTable::Type>*)v[1])->push_back({POINTER, 0, 0, true}); return v[1];};
            ;

    gen.add("type_qualifier_list")
    | "type_qualifier"
    | "type_qualifier_list type_qualifier"
            ;

    // 定义函数的参数列表
    gen.add("parameter_type_list")
    | "parameter_list"
    | "parameter_list , ..."
            ;

    gen.add("parameter_list")
    | "parameter_declaration"
    | "parameter_list , parameter_declaration"
            ;

    gen.add("parameter_declaration")
    | "declaration_specifiers declarator"
    | "declaration_specifiers abstract_declarator"
    | "declaration_specifiers"
            ;

    gen.add("identifier_list")
    | "IDENTIFIER"
    | "identifier_list , IDENTIFIER"
            ;

    gen.add("type_name")
    | "specifier_qualifier_list"
    | "specifier_qualifier_list abstract_declarator"
            ;

    gen.add("abstract_declarator")
    | "pointer"
    | "direct_abstract_declarator"
    | "pointer direct_abstract_declarator"
            ;

    gen.add("direct_abstract_declarator")
    | "( abstract_declarator )"
    | "[ ]"
    | "[ constant_expression ]"
    | "direct_abstract_declarator [ ]"
    | "direct_abstract_declarator [ constant_expression ]"
    | "( )"
    | "( parameter_type_list )"
    | "direct_abstract_declarator ( )"
    | "direct_abstract_declarator ( parameter_type_list )"
            ;

    gen.add("initializer")
    | "assignment_expression"
    | "{ initializer_list }"
    | "{ initializer_list , }"
            ;

    gen.add("initializer_list")
    | "initializer"
    | "initializer_list , initializer"
            ;

    gen.add("statement")
    | "labeled_statement"
    | "compound_statement"
    | "expression_statement"
    | "selection_statement"
    | "iteration_statement"
    | "jump_statement"
            ;

    gen.add("labeled_statement")
    | "IDENTIFIER : statement"
    | "case constant_expression : statement"
    | "default : statement"
            ;

    gen.add("compound_statement")
    | "{ }" |ATTR{ quat(OP::NOP, 0, 0, 0); return new size_t(QL.size() - 1); }  // 生成空四元式占位
    | "{ statement_list }"| ATTR{ return v[1]; }
    | "{ declaration_list }" | ATTR{return v[1]; }
    | "{ declaration_list statement_list }" | ATTR{return v[1]; }
            ;

    gen.add("declaration_list")
    | "declaration"
    | "declaration_list declaration"
            ;

    gen.add("statement_list")
    | "statement"
    | "statement_list statement"
            ;

    gen.add("expression_statement")
    | ";"
    | "expression ;" | ATTR{ return v[1]; }
            ;

    gen.add("selection_statement")
    | "if ( expression ) statement"| attr_endif
    | "if ( expression ) statement else statement"| attr_endif
    | "switch ( expression ) statement"
            ;

    gen.add("iteration_statement")
    | "while ( expression ) statement" | attr_endwhile
    | "do statement while ( expression ) ;"
    | "for ( expression_statement expression_statement ) statement"| attr_endfor
    | "for ( expression_statement expression_statement expression ) statement"| attr_endfor
            ;

    gen.add("jump_statement")
    | "goto IDENTIFIER ;"
    | "continue ;"
    | "break ;"
    | "return ;"| ATTR{quat(OP::RET, 0, 0, 0);}
    | "return expression ;"
            ;

    gen.add("translation_unit")
    | "external_declaration"
    | "translation_unit external_declaration"
            ;

    gen.add("external_declaration")
    | "function_definition"
    | "declaration"
            ;

    gen.add("function_definition")
    | "declaration_specifiers declarator declaration_list compound_statement"
    | "declaration_specifiers declarator compound_statement"
    | "declarator declaration_list compound_statement"
    | "declarator compound_statement"
            ;
    return gen;
}

Generators Grammar::TestLR1() {
    Generators gen;
    gen.set_non_terminators("S A E");
    gen.set_terminators("; id := +");
    gen.set_start("S");
    gen << "S" >> "S ; A";
    gen << "S" >> "A";
    gen << "A" >> "E";
    gen << "A" >> "id := E";
    gen << "E" >> "E + id";
    gen << "E" >> "id";
    return gen;
}
