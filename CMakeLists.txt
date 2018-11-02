cmake_minimum_required(VERSION 3.10)
project(Complie)

set(CMAKE_CXX_STANDARD 11)

add_executable(Complie main.cpp WordAnalysis.cpp WordAnalysis.h PrefixTree.cpp PrefixTree.h NumericDFA.cpp NumericDFA.h Utility.cpp Utility.h)