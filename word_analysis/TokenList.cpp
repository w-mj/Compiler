//
// Created by wmj on 18-11-26.
//

#include "TokenList.h"
#include <iostream>

using namespace std;

ostream& operator<<(ostream& out, const Token& s)
{
    out << "(" << s.first << ", " << s.second << ")";
    return out;
}

Token::Token(char c, int i): first(c), second(i) {

}


std::string TokenList::get_token(const Token& t) const{
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

char TokenList::get_token_char(const Token &t) const {
    if (t.first == 'h')
        return character[t.second];
    throw runtime_error(t.first + " is not a char.");
}

Number TokenList::get_token_num(const Token &t) const {
    if (t.first == 'c')
        return constants[t.second];
    throw runtime_error(t.first + " is not a number.");
}

void TokenList::add_id(const std::string &a) {
    list.emplace_back('i', id.size());
    id.push_back(a);
}

void TokenList::add_char(char a) {
    list.emplace_back('h', character.size());
    character.push_back(a);
}

void TokenList::add_str(const std::string &a) {
    list.emplace_back('s', str.size());
    str.push_back(a);
}

void TokenList::add_num(const Number &a) {
    list.emplace_back('c', constants.size());
    constants.push_back(a);
}

void TokenList::add_key(const std::string &a) {
    list.emplace_back('k', key.size());
    key.push_back(a);
}

void TokenList::add_bound(const std::string &a) {
    list.emplace_back('p', bound.size());
    bound.push_back(a);
}

void TokenList::set_bound_list(const std::vector<std::string> &l) {
    bound.clear();
    bound.insert(bound.end(), l.begin(), l.end());
}

void TokenList::set_key_list(const std::vector<std::string> &l) {
    key.clear();
    key.insert(key.end(), l.begin(), l.end());
}


void TokenList::print_token(long s, long e) const{
    if (e < 0)
        e = size() + e + 1;
    for (auto i = static_cast<size_t>(s); i < e; i++){
        Token t = list[i];
        cout << t << "    ";
        switch (t.first) {
            case 'i':
                cout << ("identification: ");
                cout << (id[t.second]);
                break;
            case 'h':
                cout << ("character: ");
                cout << (character[t.second]);
                break;
            case 's':
                cout << ("string: ");
                cout << (str[t.second]);
                break;
            case 'c':
                cout << ("constant: ");
                cout << constants[t.second];
                break;
            case 'k':
                cout << ("keyword: ");
                cout << (key[t.second]);
                break;
            case 'p':
                cout << ("bound: ");
                cout << (bound[t.second]);
                break;
            default:break;
        }
        cout << endl;
    }
}

size_t TokenList::size() const {
    return list.size();
}

TokenList::iterator TokenList::begin() {
    return list.begin();
}

TokenList::iterator TokenList::end() {
    return list.end();
}

std::string TokenList::get_grammar_token(const TokenList::iterator &it) {
    return get_grammar_token(it, end());
}

std::string TokenList::get_grammar_token(const TokenList::iterator &it, const TokenList::iterator &end) {
    if (it == end)
        return "#";
    if (it->first == 'k')
        return key[it->second];
    if (it->first == 'p')
        return bound[it->second];
    return "i";
}

