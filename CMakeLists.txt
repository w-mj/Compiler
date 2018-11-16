cmake_minimum_required(VERSION 3.10)
project(Complie)

set(CMAKE_CXX_STANDARD 11)

add_executable(Complie main.cpp
        word_analysis/WordAnalysis.cpp
        word_analysis/WordAnalysis.h
        word_analysis/PrefixTree.cpp
        word_analysis/PrefixTree.h
        word_analysis/NumericDFA.cpp
        word_analysis/NumericDFA.h
        Utility.cpp
        Utility.h
        grammar_analysis/LR1_DFA.cpp
        grammar_analysis/LR1_DFA.h
        grammar_analysis/Generators.cpp
        grammar_analysis/Generators.h
        grammar_analysis/LR1.cpp
        grammar_analysis/LR1.h)