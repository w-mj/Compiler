//
// Created by wmj on 18-11-27.
//

#include "Grammar.h"
#include "../Utility.h"
#include "Attribute/Attr_ExpQuat_2_LR.h"


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

Generators Grammar::C_Grammar() {
    Generators gen;

    gen.set_terminators("int_const float_const id ; , > < >= <= == != + - * / && || . ! "
                        "int float ( ) [ ] { } struct return if else while");
    gen.set_non_terminators("SEMI COMMA ASSIGNOP RELOP MINUS PLUS STAR DIV AND OR NOT"
                            "DOT TYPE LP RP LB RB LC RC STRUCT RETURN IF ELSE WHILE");
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
    gen << "ExtDef" >> "Specifier ExtDecList ;" | "Specifier ;" | "Specifier FunDec CompSt";
    gen << "ExtDecList" >> "VarDec" | "VarDec , ExtDecList";

    // Sepcifiers
    gen << "Specifier" >> "TYPE" | "StructSpecifier";
    gen << "StructSpecifier" >> "struct OptTag { DefList }" | "struct id";
    gen << "OptTag" >> "ID" | gen.get_epsilon();
    // gen << "Tag" >> "ID";

    //Declarators
    gen << "VarDec" >> "id" | "VarDec [ int_const ]";
    gen << "FunDec" >> "id ( VarList )" | "ID ( )";
    gen << "VarList" >> "ParamDec , VarList" | "ParamDec";
    gen << "ParamDec" >> "Specifier VarDec";

    // Local Definitions
    gen << "DefList" >> "Def DefList" | gen.get_epsilon();
    gen << "Def" >> "Specifier DecList ;";
    gen << "DecList" >> "Dec" | "Dec , DecList";
    gen << "Dec" >> "VarDec" | "VarDec ASSIGNOP Exp";

    // Expressions
    gen << "EXP" >> "id + id";

    return gen;
}

Generators Grammar::Basic_Exp_Quat(TokenList &tokenList) {
    Generators gen;

    Attribute *attr = new Attr_ExpQuat_2_LR(tokenList);
    gen.set_attr_builder(attr);

    gen.set_terminators({"+", "-", "*", "/", "(", ")", "i"});
    gen.set_non_terminators({"E", "T", "F"});
    gen.set_start("E");

    gen.add("E") >> "E + T" | attr |
                    "E - T" | attr |
                    "T";
    gen.add("T") >> "T * F" | attr |
                    "T / F" | attr |
                    "F";
    gen.add("F") >> "( E )" | new process_B |
                    "i";

    return gen;
}

Generators Grammar::C_Exp() {
    Generators gen;

    gen.set_terminators("= += -= *= /= %= <<= >>= &= ^= |= ? : "
                        "|| && | ^ & == != <= >= > < << >> + - *"
                        " / % ++ -- ! ~ ( ) sizeof [ ] . -> i t c s ,");
    gen.set_non_terminators("Exp AssignExp ConditionalExp UnaryExp AssignOperator BinaryExp PostfixExp UnaryExp "
                            "PostfixOperator PriExp BinaryOperator UnaryOperator Exp_A Cond_A Bin_A Post_A PostOP_A PostOP_B");

    gen << "Exp" >> "AssignExp Exp_A";
    gen << "Exp_A" >> ", AssignExp Exp_A" | gen.get_epsilon();

    gen << "AssignExp" >> "ConditionalExp"|
                            "UnaryExp AssignOperator AssignExp";
    gen << "AssignOperator" >> "="| "+=" | "-=" | "*=" | "/=" | "%=" | "<<=" | ">>=" | "&=" | "^=" | "|=";

    gen << "ConditionalExp" >> "BinaryExp Cond_A";
    gen << "Cond_A" >> "? Exp : ConditionalExp"| gen.get_epsilon();

    gen << "BinaryExp" >> "UnaryExp Bin_A";
    gen << "Bin_A" >> "BinaryOperator UnaryExp Bin_A"| gen.get_epsilon();

    gen << "BinaryOperator" >> "||"| "&&"| "|" | "^"| "&"| "==" | "!=" | "<" | ">" | "<=" | ">=" | "<<"| ">>"| "+" |"-" | "*"| "/" |"%";

    gen << "UnaryExp" >> "PostfixExp"| "UnaryOperator UnaryExp"| "( t ) UnaryExp" | "sizeof UnaryExp" | "sizeof ( t )";

    gen << "UnaryOperator" >> "++"| "--"| "&" | "*" | "+" | "-" | "~" | "!";

    gen << "PostfixExp" >> "PriExp Post_A";
    gen << "Post_A" >> "PostfixOperator Post_A"| gen.get_epsilon();

    gen << "PostfixOperator" >> "[ Exp ]"| "( PostOP_A )"| ". i"| "-> i"| "++" | "--";
    gen << "PostOP_A" >> "AssignExp PostOP_B"| gen.get_epsilon();
    gen << "PostOP_B" >> ", AssignExp PostOP_B"| gen.get_epsilon();

    gen << "PriExp" >> "i"| "c"| "s" | "( Exp )";

    return gen;
}
