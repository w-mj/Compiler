cmake_minimum_required(VERSION 3.13)
project(Compile)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)
include_directories(error)
include_directories(grammar_analysis)
include_directories(object_code)
include_directories(optimizer)
include_directories(preprocess)
include_directories(symbol_table)
include_directories(word_analysis)

add_executable(Compile
        error/Error.cpp
        error/Error.h
        grammar_analysis/Attribute.cpp
        grammar_analysis/Attribute.h
        grammar_analysis/Generators.cpp
        grammar_analysis/Generators.h
        grammar_analysis/Grammar.cpp
        grammar_analysis/Grammar.h
        grammar_analysis/GrammarTree.cpp
        grammar_analysis/GrammarTree.h
        grammar_analysis/LL1.cpp
        grammar_analysis/LL1.h
        grammar_analysis/LR1.cpp
        grammar_analysis/LR1.h
        grammar_analysis/LR1_DFA.cpp
        grammar_analysis/LR1_DFA.h
        object_code/8086.cpp
        object_code/8086.h
        object_code/80862.cpp
        object_code/80862.h
        optimizer/optimizer.cpp
        optimizer/optimizer.h
        preprocess/Fileloader.cpp
        preprocess/Fileloader.h
        preprocess/Preprocessor.cpp
        preprocess/Preprocessor.h
        symbol_table/Quaternary.cpp
        symbol_table/Quaternary.h
        symbol_table/SymbolTable.cpp
        symbol_table/SymbolTable.h
        word_analysis/NumericDFA.cpp
        word_analysis/NumericDFA.h
        word_analysis/PrefixTree.cpp
        word_analysis/PrefixTree.h
        word_analysis/TokenList.cpp
        word_analysis/TokenList.h
        word_analysis/WordAnalysis.cpp
        word_analysis/WordAnalysis.h
        main.cpp
        Utility.cpp
        Utility.h)
