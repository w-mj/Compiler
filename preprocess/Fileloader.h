//
// Created by WU on 2018/12/18.
//

#ifndef PREPROCESS_FILELOADER_H
#define PREPROCESS_FILELOADER_H
//
// Created by WU on 2018/12/18.
//

#ifndef PRE_PROCESS_FILELOADER_H
#define PRE_PROCESS_FILELOADER_H

#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <string>

using namespace std;

class Fileloader{
private:
    ifstream file_stack[50];
    int top;
    unsigned int line;
public:
    Fileloader(string file_name);

    void push_file(string file_name);
    void pop_file();

    bool is_empty();
    ifstream& get_top();

    bool getfileline(string &str);

    unsigned int get_line();
};

#endif //PRE_PROCESS_FILELOADER_H

#endif //PREPROCESS_FILELOADER_H
