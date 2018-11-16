//
// Created by wmj on 18-11-1.
//

#include "WordAnalysis.h"
#include <algorithm>
#include <string>
#include <cmath>
#include "../Utility.h"

using namespace std;

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
    result.append("{");
    result.push_back(t.first);
    result.append(", " + to_string(t.second) + "}    ");
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
            result.append(constants[t.second].str());
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
    } else iter = start;
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
    iter += 1; // skip first ""
    string::iterator start = iter;
    string result;

    while (*iter != '"' || *(iter - 1) == '\\') {
        // *end = '\0', so it is no use to check if iter == end
        if (*iter != '\\') {
            result.push_back(*iter);
            iter += 1;
        } else {
            iter += 1; // skip /
            if (*iter == 'x' || *iter == 'X' || (*iter >= 1 && *iter <= 7) ||
                (*iter == '0' && isoct(*iter + 1) && isoct(*iter + 2))) {
                result.push_back(static_cast<char>(getEscape(iter, iter + 3)));
                iter = iter + 3;
            } else {
                result.push_back(static_cast<char>(getEscape(iter, iter + 1)));
                iter = iter + 1;
            }
        }
    }

    if (iter == end)
        throw runtime_error("missing terminating \" character");
    str.push_back(result);
    token.emplace_back('s', str.size() - 1);
    iter ++; // skip the last "
    return true;
}

bool WordAnalysis::process_char(std::string::iterator &iter, const std::string::iterator &end) {
    int c = 0;
    iter += 1; // skip '
    string::iterator f = iter;
    while (f != end && (*f != '\'' || *(f-1) == '\\')) f++;
    if (f == end)
        throw runtime_error("missing terminating ' character");
    if (f - iter == 0)
        throw runtime_error("empty character constant");
    if (*iter == '\\')
        c = getEscape(iter + 1, f);
    else if (f - iter == 1)
        c = *iter;
    else
        throw runtime_error("multi-character character constant");


    character.push_back(c);
    token.emplace_back('h', character.size() - 1);
    iter = f + 1;  // skip the char and the last quotes
    return true;
}

bool WordAnalysis::process_constant(std::string::iterator &iter, const std::string::iterator &end) {
    static NumericDFA dfa;
    constants.push_back(dfa.match(iter, end));
    token.emplace_back('c', constants.size() - 1);
    return true;
}

bool WordAnalysis::process_bound(std::string::iterator &iter, const std::string::iterator &end) {
    string::iterator start = iter;
    bool m = bound_tree.match(iter, end);
    if (m)
        token.emplace_back(make_pair('p', find(bound.begin(), bound.end(), string(start, iter)) - bound.begin()));
    else
        throw runtime_error("Unresolvable character " + string(start, iter + 1));
    return m;
}

int WordAnalysis::getEscape(string::iterator s, string::iterator e) {
    if (e - s == 1)
        switch (*s) {
            case 'a': return 7;
            case 'b': return 8;
            case 'f': return 12;
            case 'n': return 10;
            case 'r': return 13;
            case 't': return 9;
            case 'v': return 11;
            case '\\': return 92;
            case '\'': return 39;
            case '"': return 34;
            case '0': return 0;
            default: break;
        }
    if (e - s == 3) {
        if (isoct(*s) && isoct(*(s + 1)) && isoct(*(s + 2))) {
            int a = *s - '0';
            int b = *(s + 1) - '0';
            int c = *(s + 2) - '0';
            return (a << 6) + (b << 3) + c;
        }
        if ((*s == 'x' || *s == 'X') && ishex(*(s + 1)) && ishex(*(s + 2))) {
            int a = hex2dec(*(s + 1));
            int b = hex2dec(*(s + 2));
            return (a << 4) + b;
        }

    }
    throw runtime_error("unknown escape sequence: \\" + string(s, e));
}