//
// Created by wmj on 18-11-1.
//

#ifndef COMPLIE_NUMERICDFA_H
#define COMPLIE_NUMERICDFA_H

#include <string>

struct Number {
    enum NumberType {UShort, Short, UInt, Int, ULong, Long, Float, Double} type;
    union {
        unsigned short us;
        signed short ss;
        unsigned int ui;
        signed int si;
        unsigned long ul;
        signed long sl;
        float ft;
        double db;
    } value;
    std::string str();
};

class NumericDFA {
private:
    bool available(char c);
public:
    Number match(std::string::iterator &iter, const std::string::iterator &end);
};


#endif //COMPLIE_NUMERICDFA_H
