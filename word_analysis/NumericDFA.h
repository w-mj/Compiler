//
// Created by wmj on 18-11-1.
//

#ifndef COMPLIE_NUMERICDFA_H
#define COMPLIE_NUMERICDFA_H

#include <string>

struct Number {
    enum NumberType {UShort, Short, UInt, Int, ULong, Long, Float, Double} type;
    union U{
        unsigned long ul;
        unsigned short us;
        signed short ss;
        unsigned int ui;
        signed int si;
        signed long sl;
        float ft;
        double db;
    } value;
    std::string str() const;
    friend std::ostream& operator<<(std::ostream& out, const Number& s);
};

class NumericDFA {
private:
    bool available(char c);
public:
    Number match(std::string::iterator &iter, const std::string::iterator &end);
};

std::ostream& operator<<(std::ostream& out, const Number& s);



#endif //COMPLIE_NUMERICDFA_H
