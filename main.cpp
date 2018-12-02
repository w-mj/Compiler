#include <iostream>
#include <fstream>
#include <string>
#include <zconf.h>
#include "word_analysis/WordAnalysis.h"
#include "grammar_analysis/Generators.h"
#include "grammar_analysis/LR1.h"
#include "grammar_analysis/LL1.h"
#include "grammar_analysis/Grammar.h"

using namespace std;

int main() {

    char path[100];
    getcwd(path, 100);
    string ps = string(path);
    if (ps.find("cmake-build-debug"))
        chdir("../");

    string fname = "test.txt";
    ifstream file;
    file.open(fname);
    string line;
    WordAnalysis analyzer;

    analyzer.process_file(file, false);
    file.close();

    Generators basic_Exp = Grammar::Basic_Exp();
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
    generators.show();
    // LR1 lr1(generators, tkl);
    LR1 lr1(generators, tkl, "lr1_table");
    // lr1.save("lr1_table");
    cout << "finish building LR1 table" << endl;
    lr1.show();
    TokenGetter getter(tkl);
    lr1.process(getter);
    cout << endl << " LR1 OK" << endl << endl;


    return 0;
}