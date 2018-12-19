//
// Created by wmj on 18-12-4.
//

#include "Error.h"
#include "../Utility.h"
#include <iostream>

using namespace std;

void error(const std::string& s) {
    cout << s << endl;
    exit(100);
}

void warring(const std::string& s) {
    cout << s << endl;
}

void rterr(const std::string& s) {
    throw runtime_error(debugpos + " " + s);
}