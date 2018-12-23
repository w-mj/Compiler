//
// Created by WU on 2018/12/18.
//

#ifndef PREPROCESS_PREPROCESSOR_H
#define PREPROCESS_PREPROCESSOR_H

#include <vector>
#include <map>
#include <iostream>
#include <set>
#include "Fileloader.h"

using namespace std;

class Preprocess{
private:
    string file;
    string file_name;
    string  file_path1,file_path2;
    set<string> include_list;
    map<string,string> define_content;
    map<string,vector<string>> define_param;
    Fileloader file_reader;
public:
    explicit Preprocess(string file_name);

    void preprocess();

    string replace(string& id,vector<string>& param_list);
    string replace(const string& new_str,string& old_str, const string& replace_str);

    void set_path1(string &path);
    void set_path2(string &path);

    bool is_define(string::iterator &iter,const string::iterator &end);
    bool is_undef(string::iterator &iter,const string::iterator &end);
    bool is_include(string::iterator &iter,const string::iterator &end);
    bool is_comment(string::iterator &iter,const string::iterator &end,unsigned int &flag);
    bool if_define(string& nowline,string::iterator &iter,const string::iterator &end);

    string define(string::iterator &iter,const string::iterator &end, bool& define_not_end, const string& define_left
                  ,bool& define_first_time);
    void include(string::iterator &iter,const string::iterator &end);
    void undef(string::iterator &iter,const string::iterator &end);


    void include_file(string file_name);

    void output();
    void out_string(string str);
};
#endif //PREPROCESS_PREPROCESSOR_H
