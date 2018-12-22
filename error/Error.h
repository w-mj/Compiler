//
// Created by wmj on 18-12-4.
//

#ifndef COMPLIE_ERROR_H
#define COMPLIE_ERROR_H

#include <string>
#include "../Utility.h"

void error(const std::string& s);
void error(const std::string& s, int line, int s1, int e1);
void error(const std::string& s, int line, int s1, int e1, int s2, int e2);

void warring(const std::string& s);
void warring(const std::string& s, int line, int s1, int e1);
void warring(const std::string& s, int line, int s1, int e1, int s2, int e2);

void print_file(int line, int s1, int e1);
void print_file(int line, int s1, int e1, int s2, int e2);
#define rterr(s) (throw runtime_error(debugpos + " " + (s)))



#endif //COMPLIE_ERROR_H
