//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_LR0_GENERATORS_H
#define COMPLIE_LR0_GENERATORS_H
#include <string>
#include <vector>
#include <map>
#include <set>

class LR0_Generators {
    
    std::vector<std::string> terminators;
    std::vector<std::string> non_terminators;
    std::map<std::string, std::vector<std::string>> generators;

public:
    LR0_Generators(const std::vector<std::string>& terminators, const std::vector<std::string>& non_terminators);
    void load_text(std::string name);
};


#endif //COMPLIE_LR0_GENERATORS_H
