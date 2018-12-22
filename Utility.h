//
// Created by wmj on 18-11-2.
//

#ifndef COMPLIE_UTILITY_H
#define COMPLIE_UTILITY_H

#define ishex(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define isoct(c) ((c) >= '0' && (c) <= '7')
#define isdec(c) ((c) >= '0' && (c) <= '9')

#include <algorithm>
#define in_vec(v, a) (find(((v).begin()), ((v).end()), a) != ((v).end()))
#define in_set(v, a) ((v).find(a) != (v).end())
#define last_vec(v) ((v)[(v).size() - 1])

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <cstdlib>

#define debugpos std::string(__FILE__) + " " + std::to_string( __LINE__) + " " + std::string(__FUNCTION__)

int hex2dec(char c);
bool isalnumunder(int c);
std::vector<std::string> split(const std::string &str, char sp=' ');
std::set<std::string> v2s(std::vector<std::string>& s);

template<typename T>
bool union_set(std::set<T> &s1, std::set<T> &s2) {
    size_t o = s1.size();
    set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), inserter(s1, s1.begin()));
    return o != s1.size();
}

template<typename T>
bool insert_set(std::set<T> &s1, T s2) {
    size_t o = s1.size();
    s1.insert(s2);
    return o != s1.size();
}

template <typename T>
std::ostream& operator<<(std::ostream& out, std::set<T>& s) {
    out << "{";
    size_t cnt = 0, last = s.size() - 1;
    for (auto &x: s) {
        if (cnt != last)
            out << x << ", ";
        else
            out << x;
        cnt++;
    }
    out << "}";
    return out;
}

template<typename T, typename ...Args>
bool oneof(T a, Args... args) {
    return ((a == args) || ...);
};

#ifdef __linux__
#define COLOR_RED 1
#define COLOR_BLUE 2
#endif

#ifdef __WIN32
#define COLOR_RED 0x0c
#define COLOR_BLUE 0x09
#endif

void colorful_print(const std::string& s, int color, char end='\n');


#endif //COMPLIE_UTILITY_H
