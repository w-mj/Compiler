//
// Created by wmj on 18-11-15.
//

#ifndef COMPLIE_Generators_H
#define COMPLIE_Generators_H
#include <string>
#include <vector>
#include <map>
#include <set>
#include <iostream>

#define get_gen(A, n) (g_list[g_map[A][n]])

typedef std::string generator_A;
typedef std::vector<std::string> generator_B;
typedef std::pair<generator_A, generator_B> generator;


class Generators {

    std::set<generator_A> terminators;
    std::set<std::string> non_terminators;
    std::map<generator_A, std::vector<size_t>> g_map;
    std::vector<generator> g_list;
    generator_A start;
    const generator_A epsilon = "\\e";
    const generator_A end = "#";
    const generator_A deleted = "deleted";

    std::map<generator_A, std::set<std::string>> first_set;
    std::map<generator_A, std::set<std::string>> follow_set;

    void build_first_set();
    void build_follow_set();


    bool built_first = false;
    bool built_follow = false;

public:
    Generators();
    void load_text(std::string name);
    size_t add_generator(const generator& g);
    size_t add_generator(const generator_A& A, const generator_B& B);
    void remove_generator(const generator_A& A, size_t n);
    std::vector<size_t> get_generators_index(const generator_A& A);
    std::set<std::string> first(const std::string& A);
    std::set<std::string> follow(const std::string& B);
    bool isVN(const std::string& s) const;
    bool isVT(const std::string& s) const;
    void show();
    std::string to_str();
    generator_A get_start() const;
    generator_A get_end() const;
    generator_A get_epsilon() const;

    std::vector<generator_A> get_terminators();
    std::vector<std::string> get_non_terminators();
    const generator& operator[](size_t i);
    size_t size() const;
    void remove_left_recursive();
    bool exists(size_t t);

    size_t non_terminator_size() const;
    size_t terminator_size() const;


    void _print_first();
    void _print_follow();


    struct GeneratorAdder {
        generator_A A;
        Generators& gens;
        friend GeneratorAdder& operator<<(GeneratorAdder& adder, const generator_B& B);
    };

};

generator_B make_generator_B(const std::string &s);
generator make_generator(const std::string &A, const std::string &B);

void show_generator(const generator& g);

std::ostream& operator<<(std::ostream& out, const generator &g);
Generators::GeneratorAdder& operator<<(Generators::GeneratorAdder& adder, const generator_B& B);


#endif //COMPLIE_Generators_H
