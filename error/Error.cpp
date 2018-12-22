//
// Created by wmj on 18-12-4.
//

#include "Error.h"
#include "../Utility.h"
#include <iostream>
#include <fstream>

using namespace std;

extern string fname;
extern int fline, fstart_pos, fend_pos;

void error(const std::string& s) {
    colorful_print(s, COLOR_RED);
    print_file(fline, fstart_pos, fend_pos);
    exit(100);
}

void error(const std::string& s, int line, int s1, int e1) {
    error(s);
    print_file(line, s1, e1);
}

void error(const std::string& s, int line, int s1, int e1, int s2, int e2) {
    error(s);
    print_file(line, s1, e1, s2, e2);
}

void print_file(int line, int s1, int e1) {
    cout << "\033[1mat " << fname << " " << fline << ":" << fstart_pos << endl;
    ifstream fs;
    string buff;
    fs.open(fname);
    for (int i = 0; i < line; i++) {
        getline(fs, buff);
    }
    cout << buff << endl;
    for (size_t i = 0; i < buff.size(); i++) {
        if (i == s1)
            cout << "^";
        else if (i > s1 && i < e1)
            cout << "^";
        else
            cout << '~';
    }
    cout << endl << "\033[0m";
    fs.close();
}

void print_file(int line, int s1, int e1, int s2, int e2) {
#ifdef __linux__
    cout << "\033[1m";
#endif
    cout << "at " << fname << " " << fline << ":" << fstart_pos << endl;
    ifstream fs;
    string buff;
    fs.open(fname);
    for (int i = 0; i < line; i++) {
        getline(fs, buff);
    }
    cout << buff << endl;
    for (size_t i = 0; i < buff.size(); i++) {
        if ((i == s1 && i != e1) || (i == s2 && i != e2))
            cout << "^";
        else if ((i > s1 && i < e1) || (i > s2 && i < e2))
            cout << "^";
        else
            cout << '~';
    }
#ifdef __linux__
    cout << endl << "\033[0m";
#endif
    fs.close();
}

void warring(const std::string& s) {
    colorful_print(s, COLOR_BLUE);
    print_file(fline, fstart_pos, fend_pos);
}

void warring(const std::string& s, int line, int s1, int e1) {
    warring(s);
    print_file(line, s1, e1);
}

void warring(const std::string& s, int line, int s1, int e1, int s2, int e2) {
    warring(s);
    print_file(line, s1, e1, s2, e2);
}
