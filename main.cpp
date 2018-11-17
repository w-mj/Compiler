#include <iostream>
#include <fstream>
#include <string>
#include "word_analysis/WordAnalysis.h"
#include "grammar_analysis/Generators.h"
#include "grammar_analysis/LR1.h"

using namespace std;

int main() {
    string fname = "../test.txt";
    ifstream file;
    file.open(fname);
    string line;
    WordAnalysis analyzer;
    Generators generators;
    generators.load_text("../grammar_analysis/test");
    // generators.show();

    analyzer.process_file(file, false);

    LR1 lr1(generators, analyzer);
    // lr1.show();
    lr1.process();

    return 0;
}