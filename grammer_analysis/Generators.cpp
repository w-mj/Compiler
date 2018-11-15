//
// Created by wmj on 18-11-15.
//

#include "Generators.h"
#include <fstream>
#include <iostream>
#include "../Utility.h"

using namespace std;

Generators::Generators(const std::vector<std::string>& terminators,
                               const std::vector<std::string>& non_terminators):
    terminators(terminators), non_terminators(non_terminators)
{

}

void Generators::load_text(std::string name) {
    ifstream fin;
    fin.open(name);
    string buffer;
    while (getline(fin, buffer)) {
        vector<string> gens = split(buffer, '`');
        string A = gens[0];
        for (int i = 1; i < gens.size(); i++) {
            auto B = split(gens[i]);
            if (generators.find(A) == generators.end())
                generators.insert(make_pair(A, vector<vector<string>>()));
            generators[A].push_back(B);
        }
    }

    for (auto x: generators) {
        for (auto b: x.second) {
            cout << x.first << "  ->  ";
            for (auto c: b)
                cout <<  c;
            cout << endl;
        }
        cout << endl;
    }
}


int main1() {
     vector<string> a;
     Generators g(a, a);
     g.load_text("../LR0_analysis/expression_analytic");
}