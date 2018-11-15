//
// Created by wmj on 18-11-15.
//

#include "LR0_Generators.h"
#include <fstream>
#include <iostream>

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
        
    }

}
