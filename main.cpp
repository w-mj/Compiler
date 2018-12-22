#include <iostream>
#include <fstream>
#include <string>

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
#include "object_code/8086.h"
#include "object_code/80862.h"

using namespace std;
string fname;
int fline, fstart_pos, fend_pos;

int main(int argc, char** argv) {
    char path[100];
    getcwd(path, 100);
    string ps = string(path);
    if (ps.find("cmake-build-debug"))
        chdir("../");

    if (argc == 1) {
        cout << "Fatal: no input file" << endl;
        exit(2);
    }
    fname = string(argv[1]);
    // fname = "test.src";
    ifstream file;
    file.open(fname);
    string line;
    WordAnalysis& analyzer = WA;

    analyzer.process_file(file, false);
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
    cout << endl << " LR1 OK" << endl << endl;

    cout << QL << endl << endl;

    cout << ST << endl;

    cout << endl;
    cout << endl;

    // make8086();
    // makeasm();
    optimizer o;

    return 0;
}