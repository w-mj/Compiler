//
// Created by wmj on 18-11-1.
//

#include "WordAnalysis.h"
#include <algorithm>
#include <string>
#include <cmath>

using namespace std;


bool isalnumunder(int c) {
    return isalnum((c)) || (c) == '_';
}

WordAnalysis::WordAnalysis():
    key{"char", "double", "enum", "float", "int", "long", "short", "signed", "struct",
        "union", "unsigned", "void", "for", "do", "while", "break", "continue", "if",
        "else", "goto", "switch", "case", "default", "return", "auto", "extern", "register",
        "static", "const", "sizeof", "typedef", "volatile"},
    bound{"++", "--", "(", ")", "[", "]", ".", "->", "+", "-", "!", "~", "&",
          "*", "/", "%", "<<", ">>", "<", "<=", ">", ">=", "==", "!=", "^", "|",
          "&&", "||", "?", ":", "=", "+=", "-=", "*=", "/=", "%=", "<<=", ">>=",
          "&=", "^=", "|=", ",", ";", "{", "}"},
    key_tree(key), bound_tree(bound) {
}

std::string WordAnalysis::token2str(long i) {
    if (i < 0)
        i = token.size() + i;
    pair<char, int> t = token[i];
    std::string result;
    switch (t.first) {
        case 'i':
            result.append("identification: ");
            result.append(id[t.second]);
            break;
        case 'h':
            result.append("character: ");
            result.push_back(character[t.second]);
            break;
        case 's':
            result.append("string: ");
            result.append(str[t.second]);
            break;
        case 'c':
            result.append("constant: ");
            result.append(to_string(constants[t.second]));
            break;
        case 'k':
            result.append("keyword: ");
            result.append(key[t.second]);
            break;
        case 'p':
            result.append("bound: ");
            result.append(bound[t.second]);
            break;
        default:break;
    }
    return result;
}

bool WordAnalysis::process_key(std::string::iterator &iter, const std::string::iterator& end) {
    string::iterator start = iter;
    bool m = key_tree.match(iter, end, isalnumunder);
    if (m) {
        token.emplace_back('k', find(key.begin(), key.end(), string(start, iter)) - key.begin());
    }
    return m;
}

bool WordAnalysis::process_id(std::string::iterator &iter, const std::string::iterator &end) {
    string::iterator start = iter;
    bool m = id_tree.insert(iter, end, isalnumunder);
    if (m) {
        id.emplace_back(start, iter);
    }
    token.emplace_back('i', find(id.begin(), id.end(), string(start, iter)) - id.begin());
    return true;
}

const std::vector<std::pair<char, int>> &WordAnalysis::getToken() {
    return token;
}

bool WordAnalysis::process_str(std::string::iterator &iter, const std::string::iterator &end) {
    // TODO: escape character
    iter += 1; // skip first ""
    string::iterator start = iter;

    while (*iter != '"')  // *end = '\0', so it is no use to check if iter == end
        iter++;

    if (iter == end || *iter != '"')
        return false;
    str.emplace_back(start, iter);
    token.emplace_back('s', str.size() - 1);
    iter ++; // skip the last "
    return true;
}

bool WordAnalysis::process_char(std::string::iterator &iter, const std::string::iterator &end) {
    // TODO: escape character
    iter += 1; // skip '
    if (iter == end || *(iter + 1) != '\'')
        return false;
    character.push_back(*iter);
    token.emplace_back('h', character.size() - 1);
    iter += 2; // skip the char and the last quotes
    return true;
}

bool WordAnalysis::process_constant(std::string::iterator &iter, const std::string::iterator &end) {
    long integer = 0, fraction = 0, cnt_fraction = 0, e_pow = 0;
    int base = 10;
    if (*iter == '0') {
        iter++;
        if (*iter == 'x') {
            base = 16;  // hexadecimal
            iter++;
        }
        else if (isdigit(*iter)) base = 8;  // octal
    }
    while (iter != end && isdigit(*iter)) {
        integer = integer * base + (*iter - '0');
        iter++;
    }
    if (base == 10 && *iter == '.') {
        iter++;
        while (iter != end && isdigit(*iter)) {
            fraction = fraction * 10 + (*iter - '0');
            cnt_fraction += 1;
            iter++;
        }
        // TODO: check cnt_fraction != 0
    }
    if (base == 10 && (*iter == 'e' or *iter == 'E')) {
        iter++;
        while (iter != end && isdigit(*iter)) {
            e_pow = e_pow * 10 + (*iter - '0');
            iter++;
        }
    }
    double result = integer + static_cast<double>(fraction) / pow(10, cnt_fraction);
    result = result * pow(10, e_pow);
    constants.push_back(result);
    token.emplace_back('c', constants.size() - 1);
    return true;
}

bool WordAnalysis::process_bound(std::string::iterator &iter, const std::string::iterator &end) {
    string::iterator start = iter;
    bool m = bound_tree.match(iter, end);
    if (m)
        token.emplace_back(make_pair('p', find(bound.begin(), bound.end(), string(start, iter)) - bound.begin()));
    return m;
}
