//
// Created by wmj on 18-11-2.
//

#ifndef COMPLIE_UTILITY_H
#define COMPLIE_UTILITY_H

#define ishex(c) (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define isoct(c) ((c) >= '0' && (c) <= '7')
#define isdec(c) ((c) >= '0' && (c) <= '9')

#include <string>
#include <vector>

int hex2dec(char c);
bool isalnumunder(int c);
std::vector<std::string> split(const std::string &str, char sp=' ');



#endif //COMPLIE_UTILITY_H
