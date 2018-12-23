#include <iostream>
#include <fstream>
#include <string>
#include <cstring>

#ifdef __linux__
#include <zconf.h>
#endif
#ifdef _WIN32
#include <io.h>
#endif

#include "word_analysis/WordAnalysis.h"
#include "grammar_analysis/Generators.h"
#include "grammar_analysis/LR1.h"
#include "grammar_analysis/LL1.h"
#include "grammar_analysis/Grammar.h"
#include "Utility.h"
#include "symbol_table/SymbolTable.h"
#include "symbol_table/Quaternary.h"
#include "optimizer/optimizer.h"
#include "preprocess/Preprocessor.h"
#include "object_code/8086.h"
#include "object_code/80862.h"

using namespace std;
string fname;
int fline, fstart_pos, fend_pos;

int main(int argc, char** argv) {
    char path[1000];
    getcwd(path, 100);
    string ps = string(path);
    cout << ps << endl;
    if (ps.find("cmake-build-debug") != string::npos) {
        cout << "chdir" << endl;
        chdir("../");
    }

    if (argc == 1) {
        cout << "Fatal: no input file" << endl;
        exit(2);
    }
    fname = string(argv[1]);
    bool opt = false;
    if (argc == 3 && strcmp(argv[2], "-o") == 0) {
        opt = true;
    }

    // fname = "test.src";
    ifstream file;

    // Preprocess p(fname);
    // string afname = split(fname, '.')[0];
    file.open(fname);

    // file.open(afname+".i");
    cout << fname << endl;
    string line;
    WordAnalysis& analyzer = WA;

    // freopen((fname + ".tokenlist").c_str(), "w", stdout);
    analyzer.process_file(file, true);
//    #ifdef __linux__
//        freopen("/dev/tty", "w", stdout);
//    #endif
//    #ifdef _WIN32
//        freopen("CON", "w", stdout);
//    #endif
    file.close();

    // Generators basic_Exp = Grammar::Basic_Exp();
    TokenList tkl = analyzer.get_tokenList();

//    basic_Exp.remove_left_recursive();
//    basic_Exp.show();
//
//    basic_Exp._print_first();
//    basic_Exp._print_follow();
//    LL1 ll1(basic_Exp, tkl);
//    ll1.build();
//    ll1.show();
//    TokenGetter getter1(tkl);
//    ll1.process(getter1);
//    cout << "LL1 OK" << endl << endl;


    Generators generators = Grammar::YACC_C_Grammar();
    // generators._print_first();
    // generators.show();
    LR1 lr1(generators, tkl);
    // lr1.build();
    // lr1.save();
    lr1.load();
    lr1.show();
    TokenGetter getter(tkl);
    lr1.process(getter);
    // cout << endl << " LR1 OK" << endl << endl;

   //freopen((fname + ".quatlist").c_str(), "w", stdout);
    cout << QL << endl << endl;

   //freopen((fname + ".symboltable").c_str(), "w", stdout);
    cout << ST << endl;

//    #ifdef __linux__
//        freopen("/dev/tty", "w", stdout);
//    #endif
//    #ifdef _WIN32
//        freopen("CON", "w", stdout);
//    #endif


    if (1) {
        cout << "Optimizer activited." << endl;
        optimizer o;
    }

    cout << QL << endl << endl;


    // make8086();
    // freopen((fname + ".asm").c_str(), "w", stdout);
    makeasm();
//#ifdef __linux__
//    freopen("/dev/tty", "w", stdout);
//#endif
//#ifdef _WIN32
//    freopen("CON", "w", stdout);
//#endif
    cout << endl << "generate token list at " << fname << ".tokenlist" << endl;
    cout << endl << "generate symbol table at " << fname << ".symboltable" << endl;
    cout << endl << "generate quaternary list table at " << fname << ".quatlist" << endl;
    cout << endl << "generate asm at " << fname << ".asm" << endl;
    fflush(stdout);

    return 0;
}