//
// Created by WU on 2018/12/18.
//

#include "Fileloader.h"

Fileloader::Fileloader(string file_name) {
    top=0;
    line=0;
    file_stack[top].open(file_name);
}

bool Fileloader::is_empty() {
    return !(top==-1);
}

ifstream& Fileloader::get_top() {
    if(top>=0){
        return file_stack[top];
    } else{
        throw runtime_error("stack is empty");
    }
}


void Fileloader::push_file(string file_name) {
    ++top;
    file_stack[top].open(file_name);
    if(!file_stack[top].is_open()){
        throw runtime_error("Can't open the file");
    }
}
void Fileloader::pop_file() {
    top--;
}

bool Fileloader::getfileline(string &str) {
     if(getline(get_top(),str)){
         if(top>0){
             line++;
         }
         return true;
     } else if(top>0){
         get_top().close();
         this->pop_file();
         getline(get_top(),str);
         if(top>0){
             line++;
         }
         return true;
     } else{
         return false;
     }
}

unsigned int Fileloader::get_line() {
    return line;
}
