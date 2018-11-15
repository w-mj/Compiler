//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_Generators_H
#define COMPLIE_Generators_H
#include <string>
#include <vector>
#include <map>
#include <set>

class Generators {
    
    std::vector<std::string> terminators;
    std::vector<std::string> non_terminators;
    std::map<std::string, std::vector<std::vector<std::string>>> generators;

public:
    Generators(const std::vector<std::string>& terminators, const std::vector<std::string>& non_terminators);
    void load_text(std::string name);
};


#endif //COMPLIE_Generators_H
