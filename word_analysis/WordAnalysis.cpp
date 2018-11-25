//
// Created by wmj on 18-11-1.
//

#include "WordAnalysis.h"
#include <algorithm>
#include <string>
#include <cmath>
#include <iostream>
#include <fstream>
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

const std::vector<std::pair<char, int>> &WordAnalysis::get_token_list() const {
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

std::string WordAnalysis::get_token(const Token& t) const{
    switch (t.first) {
        case 'i':
            return id[t.second];
        case 's':
            return str[t.second];
        case 'k':
            return key[t.second];
        case 'p':
            return bound[t.second];
        default:
            throw runtime_error("Unrecognized token type." + t.first);
    }
}

char WordAnalysis::get_token_char(const Token &t) const {
    if (t.first == 'h')
        return character[t.second];
    throw runtime_error(t.first + " is not a char.");
}

Number WordAnalysis::get_token_num(const Token &t) const {
    if (t.first == 'c')
        return constants[t.second];
    throw runtime_error(t.first + " is not a number.");
}

void WordAnalysis::process_file(std::ifstream &file, bool print) {
    string::iterator start;
    string line;
    bool in_commit = false;
    while(getline(file, line)) {
        size_t start_tk = token.size();
        if (print)
            cout << endl << "in line: " << line << endl;
        string::iterator iter = line.begin();
        while (iter != line.end()) {
            if (!in_commit && *iter == '/' && *(iter + 1) == '/')  // inline commit
                break;
            if ((*iter == '/' && *(iter + 1) == '*'))  // block commit
                in_commit = true;

            if (in_commit) {
                while (iter != line.end() && !(*iter == '*' && *(iter + 1) == '/'))
                    iter++;
                if (*iter == '*' && *(iter + 1) == '/') {
                    in_commit = false;
                    iter += 2;
                }
                continue;
            }
            if (isspace(*iter)) {
                iter++;
                continue;  // skip space characters
            }
            try {
                start = iter;
                if (isalpha(*iter)) {
                    if (process_key(iter, line.end())) {

                    } else if (process_id(iter, line.end())) {

                    } else
                        iter++;
                } else if (isdigit(*iter) || (*iter == '.' && isdigit(*(iter+1))) ) {
                    process_constant(iter, line.end());
                    token2str(-1);
                } else if (*iter == '"') {
                    process_str(iter, line.end());
                } else if (*iter == '\'') {
                    process_char(iter, line.end());
                } else {
                    if (process_bound(iter, line.end()))
                        ;
                    else
                        iter++;
                }
            } catch (runtime_error &e) {
                cout << endl << "ERROR:" << endl << line << endl;
                for (int i = 0; i < start - line.begin(); i++)
                    cout << ' ';
                for (int i = 0; i < iter - start; i++)
                    cout << '~';
                cout << "^" << endl;
                cout << e.what() << endl;
                exit(100);
            }
        }
        if (print) {
            for (size_t i = start_tk; i < token.size(); i++)
                cout << "    " << token2str(i) << endl;
        }
    }

}

