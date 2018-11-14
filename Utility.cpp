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