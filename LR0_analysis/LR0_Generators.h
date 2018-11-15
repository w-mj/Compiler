//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_LR0_GENERATORS_H
#define COMPLIE_LR0_GENERATORS_H
#include <string>
#include <vector>
#include <map>
#include <set>

class LR0_Generators {
    using namespace std;
    vector<string> terminators;
    vector<string> non_terminators;
    map<string, vector<string>> generators;

    LR0_Generators(const vector<string>& terminators, const vector<string>& non_terminators);
    void load_text(string name);
};


#endif //COMPLIE_LR0_GENERATORS_H
