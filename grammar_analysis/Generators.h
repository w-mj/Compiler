//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_Generators_H
#define COMPLIE_Generators_H
#include <string>
#include <vector>
#include <map>
#include <set>

typedef std::string generator_A;
typedef std::vector<std::string> generator_B;
typedef std::pair<generator_A, generator_B> generator;

class Generators {
    
    std::vector<std::string> terminators;
    std::vector<std::string> non_terminators;
    std::map<generator_A, std::vector<size_t>> g_map;
    std::vector<generator> g_list;
    generator_A start;

public:
    Generators(const std::vector<std::string>& terminators, const std::vector<std::string>& non_terminators);
    Generators();
    void load_text(std::string name);
    size_t add_generator(const generator& g);
    size_t add_generator(const generator_A& A, const generator_B& B);
    std::vector<size_t> get_generators_index(const generator_A& A);
    std::set<std::string> first(const std::string& A);
    bool isVN(const std::string& s) const;
    bool isVT(const std::string& s) const;
    void show();
    generator_A get_start() const;
    std::vector<generator_A> get_terminators();
    std::vector<generator_A> get_non_terminators();
    const generator& operator[](size_t i);
};

generator_B make_generator_B(const std::string &s);
generator make_generator(const std::string &A, const std::string &B);

void show_generator(const generator& g);

#endif //COMPLIE_Generators_H
