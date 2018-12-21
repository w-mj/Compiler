//
// Created by wmj on 18-11-26.
//

#include "TokenList.h"
#include "../symbol_table/Quaternary.h"
#include "../Utility.h"
#include <iostream>
#include <algorithm>

using namespace std;

ostream& operator<<(ostream& out, const Token& s)
{
    out << "(" << s.first << ", " << s.second << ")";
    return out;
}

Token::Token(size_t c, size_t i): first(c), second(i) {

}

extern int fline, fstart_pos, fend_pos;


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
    list.emplace_back('k', find(key.begin(), key.end(), a) - key.begin());
}

void TokenList::add_bound(const std::string &a) {
    list.emplace_back('p', find(bound.begin(), bound.end(), a) - bound.begin());
}

void TokenList::add_asm(const string &a) {
    // cout << "**add asm**  " << a << endl;
    list.emplace_back(TOKEN_ASM, ST.add_asm(a));
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
            case 'a':
                cout << "asm: ";
                cout << ST.get_asm(t.second);
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

std::string TokenList::get_grammar_token(const Token& it) {
    if (it.first == '#')
        return "#";
    if (it.first == 'k') {
//        static auto anyone = [](auto&& k, auto&&... args) ->bool { return ((args == k) || ...); };
//        if (anyone(key[it.second], "int", "float", "short", "double", "long", "char"))
//            return "t";
//        else
            return key[it.second];
    }
    if (it.first == 'p')
        return bound[it.second];
    if (it.first == 'c')
        return "CONSTANT";
    return "IDENTIFIER";
}

std::string TokenList::get_token_str(Token t) const {
    switch (t.first) {
        case 'i':
            return id[t.second];
        case 'h':
            return to_string(character[t.second]);
        case 's':
            return str[t.second];
        case 'c':
            return constants[t.second].str();
        case 'k':
            return key[t.second];
        case 'p':
            return bound[t.second];
        case 'a':
            return ST.get_asm(t.second);
        default:break;
    }
    return "";
}

std::string TokenList::get_identification(size_t t) {
    return id[t];
}

Number TokenList::get_number(size_t t) {
    return constants[t];
}

std::string TokenList::get_bound(size_t t) {
    return bound[t];
}

std::string TokenList::get_key(size_t t) {
    return key[t];
}

void TokenList::set_last_token_pos(int line, int start, int end) {
    last_vec(list).line = line;
    last_vec(list).start = start;
    last_vec(list).end = end;

}

TokenGetter::TokenGetter(TokenList &tkl): tkl(tkl) {
    it = tkl.begin();
    end = tkl.end();
}

Token TokenGetter::get() {
    if (it != end)
        return *it;
    return {'#', 0};
}

Token TokenGetter::next() {
    if (it->first == TOKEN_ASM) {
        while (it != end && it->first == TOKEN_ASM) {
            quat(OP::ASM, it->second, 0, 0);
            it++;
        }
        if (it != end) {
            fline = it->line;
            fstart_pos = it->start;
            fend_pos = it->end;
            return *it;
        }
    }

    if (it != end) {
        it++;
        while (it != end && it->first == TOKEN_ASM) {
            quat(OP::ASM, it->second, 0, 0);
            it++;
        }
        fline = it->line;
        fstart_pos = it->start;
        fend_pos = it->end;
        return *it;
    }
    return {'#', 0};
}
