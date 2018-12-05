//
// Created by wmj on 18-12-4.
//

#include "Error.h"
#include <iostream>

using namespace std;

void error(const std::string& s) {
    cout << s << endl;
    exit(100);
}