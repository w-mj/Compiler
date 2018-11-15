//
// Created by wmj on 18-11-15.
//

#include "LR0_Generators.h"
#include <fstream>
#include <iostream>
#include "../Utility.h"

using namespace std;

LR0_Generators::LR0_Generators(const std::vector<std::string>& terminators,
                               const std::vector<std::string>& non_terminators):
    terminators(terminators), non_terminators(non_terminators)
{

}

void LR0_Generators::load_text(std::string name) {
    ifstream fin;
    fin.open(name);
    string buffer;
    while (getline(fin, buffer)) {
        vector<string> gens = split(buffer, '`');
        string A = gens[0];
        for (int i = 1; i < gens.size(); i++)
            generators.insert(make_pair(A, split(gens[i])));
    }

    for (auto x: generators) {
        cout << x.first << "->";
        for (auto b: x.second)
            cout << b << " ";
        cout << endl;
    }
}


int main() {
    vector<string> a;
    // LR0_Generators g(a, a);
    // g.load_text("../LR0_analysis/expression_analytic");
    a = split("abc def eg");
    for (string x : a)
        cout << x << endl;
}