#include <iostream>
#include <fstream>
#include <string>
#include <zconf.h>
#include "word_analysis/WordAnalysis.h"
#include "grammar_analysis/Generators.h"
#include "grammar_analysis/LR1.h"
#include "grammar_analysis/LL1.h"

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
    Generators generators;
    generators.load_text("grammar_analysis/test");
    generators.show();

    generators._print_first();
    generators._print_follow();


    analyzer.process_file(file, true);

    TokenList tkl = analyzer.get_tokenList();

    LR1 lr1(generators, tkl);
    lr1.show();
    lr1.process();
    cout << endl << " LR1 OK" << endl << endl;

    generators.remove_left_recursive();
    generators.show();

    generators._print_first();
    generators._print_follow();
    LL1 ll1(generators, tkl);
    ll1.build();
    ll1.process();
    cout << "LL1 OK" << endl;

    return 0;
}