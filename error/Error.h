//
// Created by wmj on 18-12-4.
//

#ifndef COMPLIE_ERROR_H
#define COMPLIE_ERROR_H

#include <string>

void error(const std::string& s);
void warring(const std::string& s);
#define rterr(s) (throw runtime_error(debugpos + " " + (s)))



#endif //COMPLIE_ERROR_H
