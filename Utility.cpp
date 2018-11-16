//
// Created by wmj on 18-11-2.
//

#include "Utility.h"
#include <cctype>
#include <stdexcept>

int hex2dec(char c) {
    if (isdigit(c)) return c - '0';
    if ('a' <= c && c <= 'f') return 10 + c - 'a';
    if ('A' <= c && c <= 'F') return 10 + c - 'A';
    throw "not a hexadecimal character.";
}

bool isalnumunder(int c) {
    return isalnum((c)) || (c) == '_';
}


using namespace std;

vector<string> split(const string &str, char sp) {
    vector<string> result;
    string::const_iterator iter = str.begin();
    while (true) {
        string::const_iterator s_iter = iter;
        while (iter != str.end() && *iter != sp)
            iter++;
        result.emplace_back(s_iter, iter);
        if (iter == str.end())
            return result;
        iter++;
    }
}

