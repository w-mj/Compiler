//
// Created by wmj on 18-11-2.
//

#include "Utility.h"
#include <cctype>
#include <stdexcept>
#include <iostream>
#include <sstream>

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

static void _split(const std::string &s, char delim,
                   std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;

    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    auto it = s.cbegin();
    auto end = s.cend();
    while (it != end) {
        string buf;
        while (it != end && *it != delim) {
            buf.push_back(*it);
            it++;
        }
        elems.push_back(buf);
        while (it != end && *it == delim) {
            it++;
        }
    }
    return elems;
}