//
// Created by wmj on 18-11-1.
//

#include "NumericDFA.h"
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include "../Utility.h"
#include <iostream>

using namespace std;

string c2s(char c) {
    return string(1, c);
}

Number NumericDFA::match(std::string::iterator &iter, const std::string::iterator &end) {
    long integer = 0, fraction = 0, cnt_fraction = 0, e_pow = 0;
    int state = 0;
    bool u = false, f = false, l = false, neg_pow = false, d = false;
    vector<int> finals = {1, 2, 3, 4, 6, 9, 10, 11, 12};
    Number result{};
    char pre = *iter;
    bool quit = false;
    while (!quit) {
        char c = *iter;
        switch (state) {
            case 0:
                if (c == '0') state = 1;
                else if (c >= '1' && c <= '9') state = 4;
                else if (c == '.') state = 5;
                else throw runtime_error("expect a digit but given '" + c2s(c) + "'");
                break;
            case 1:
                if (!available(c)) quit = true;
                else if (c == 'X' || c == 'x') state = 13;
                else if (isoct(c)) state = 3;
                else if (c == '.') state = 5;
                else throw runtime_error("expect 0 1 2 3 4 5 6 7 x X but given '" + c2s(c) + "'");
                break;
            case 2:
                integer = integer * 16 + hex2dec(pre);
                if (!available(c)) quit = true;
                else if (ishex(c)) state = 2;
                else if (c == 'u' || c == 'U') state = 11;
                else if (c == 'l' || c == 'L') state = 10;
                else throw runtime_error("Unexpected '" + c2s(c) + "' in hexadecimal");
                break;
            case 3:
                integer = integer * 8 + (pre - '0');
                if (!available(c)) quit = true;
                else if (isoct(c)) state = 3;
                else if (c == 'u' || c == 'U') state = 11;
                else if (c == 'l' || c == 'L') state = 10;
                else throw runtime_error("Unexpected '" + c2s(c) + "' in octal");
                break;
            case 4:
                integer = integer * 10 + (pre - '0');
                if (!available(c)) quit = true;
                else if (isdec(c)) state = 4;
                else if (c == '.') state = 5;
                else if (c == 'e' || c == 'E') state = 7;
                else if (c == 'u' || c == 'U') state = 11;
                else if (c == 'l' || c == 'L') state = 10;
                else throw runtime_error("Unexpected '" + c2s(c) + "' in decimal number");
                break;
            case 5:
                d = true;
                if (isdec(c)) state = 6;
                else throw runtime_error("Unexpected '" + c2s(c) + "' in fraction part");
                break;
            case 6:
                fraction = fraction * 10 + pre - '0';
                cnt_fraction += 1;
                if (!available(c)) quit = true;
                else if (isdec(c)) state = 6;
                else if (c == 'f' || c == 'F') state = 12;
                else if (c == 'e' || c == 'E') state = 7;
                else throw runtime_error("Unexpected '" + c2s(c) + "' in fraction part");
                break;
            case 7:
                d = true;
                if (c == '-') state = 8;
                else if (isdec(c)) state = 9;
                else throw runtime_error("Unexpected '" + c2s(c) + "' in e-power part");
                break;
            case 8:
                neg_pow = true;
                if (isdec(c)) state = 9;
                else throw runtime_error("Unexpected '" + c2s(c) + "' in e-power part");
                break;
            case 9:
                e_pow = e_pow * 10 + pre - '0';
                if (!available(c)) quit = true;
                else if (isdec(c)) state = 9;
                else if (c == 'f' || c == 'F') state = 12;
                else if (c == 'l' || c == 'L') state = 14;
                else throw runtime_error("Unexpected '" + c2s(c) + "' in e-power part");
                break;
            case 10:
                if (!l) l = true;
                else throw runtime_error("duplicate suffix l");
                if (!available(c)) quit = true;
                else if (c == 'u' || c == 'U') state = 11;
                else throw runtime_error("Unexpected '" + c2s(c) + "' in suffix.");
                break;
            case 11:
                if (!u) u = true;
                else throw runtime_error("duplicate suffix u");
                if (!available(c)) quit = true;
                else if (c == 'l' || c == 'L') state = 12;
                else throw runtime_error("Unexpected '" + c2s(c) + "' in suffix.");
                if (!available(c)) quit = true;
                break;
            case 12:
                if (!f) f = true;
                else throw runtime_error("duplicate suffix f");
                if (!available(c)) quit = true;
                break;
            case 13:
                if (ishex(c)) state = 2;
                else throw runtime_error("expect h H but given '" + c2s(c) + "'");
                break;
            case 14:
                if (!l) l = true;
                else throw runtime_error("duplicate suffix l");
                if (!available(c)) quit = true;
                break;
            default:
                throw runtime_error("Inner Error in Numeric DFA");
        }
        pre = c;
        if (!quit) iter++;
    }
    double value = (integer + fraction * pow(10, -cnt_fraction)) * pow(10, neg_pow? -e_pow: e_pow);
    if (l && f) throw runtime_error("conflict suffix l and f");
    if (l || f) {
        if (f)
            result.type = Number::NumberType::Float;
        else if (u)
            result.type = Number::NumberType::ULong;
        else
            result.type = Number::NumberType::Long;
    } else if (d)
        result.type = Number::NumberType::Double;
    else {
        if (!u) {
            if (value > INT32_MAX || value < INT32_MIN) result.type = Number::NumberType::Long;
            else if (value > INT16_MAX || value < INT16_MIN) result.type = Number::NumberType::Int;
            else result.type = Number::NumberType::Short;
        } else {
            if (value > UINT32_MAX) result.type = Number::NumberType::ULong;
            else if (value > UINT16_MAX) result.type = Number::NumberType::UInt;
            else result.type = Number::NumberType::UShort;
        }
    }
    switch (result.type) {
        case Number::NumberType::UShort: result.value.us = static_cast<unsigned short>(value); break;
        case Number::NumberType::Short: result.value.ss = static_cast<short>(value); break;
        case Number::NumberType::UInt: result.value.ui = static_cast<unsigned int>(value); break;
        case Number::NumberType::Int: result.value.si = static_cast<int>(value); break;
        case Number::NumberType::ULong: result.value.ul = static_cast<unsigned long>(value); break;
        case Number::NumberType::Long: result.value.sl = static_cast<long>(value); break;
        case Number::NumberType::Float: result.value.ft = static_cast<float>(value); break;
        case Number::NumberType::Double: result.value.db = static_cast<double>(value); break;
    }
    return result;
}

bool NumericDFA::available(char c) {
    return isalnum(c) || c == '.';
}

std::ostream &operator<<(std::ostream &out, const Number &s) {
    switch (s.type) {
        case Number::NumberType::UShort: out << s.value.us << " unsigned short";break;
        case Number::NumberType::Short: out << s.value.ss << " short"; break;
        case Number::NumberType::UInt: out << s.value.ui << " unsigned int"; break;
        case Number::NumberType::Int: out << s.value.si << " int"; break;
        case Number::NumberType::ULong: out << s.value.ul << " unsigned long"; break;
        case Number::NumberType::Long: out << s.value.sl << " long"; break;
        case Number::NumberType::Float: out << s.value.ft << " float"; break;
        case Number::NumberType::Double: out << s.value.db << " double"; break;
    }
    return out;
}


std::string Number::str() const{
    switch (type) {
        case Number::NumberType::UShort: return to_string(value.us);
        case Number::NumberType::Short: return to_string(value.ss);
        case Number::NumberType::UInt: return to_string(value.ui);
        case Number::NumberType::Int: return to_string(value.si);
        case Number::NumberType::ULong: return to_string(value.ul);
        case Number::NumberType::Long: return to_string(value.sl);
        case Number::NumberType::Float: return to_string(value.ft);
        case Number::NumberType::Double: return to_string(value.db);
    }
}
