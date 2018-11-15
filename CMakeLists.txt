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
        grammer_analysis/LR1_DFA.cpp
        grammer_analysis/LR1_DFA.h
        grammer_analysis/Generators.cpp
        grammer_analysis/Generators.h
        grammer_analysis/LR1.cpp
        grammer_analysis/LR1.h)