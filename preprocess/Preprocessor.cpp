#include <utility>

//
// Created by WU on 2018/12/18.
//

#include "Preprocessor.h"
#include <regex>

#define jmp_space(x) {\
    if(*x==' '){\
    x++;\
    }\
}


Preprocess::Preprocess(string file_name):file_reader(file_name) {
    this->file_name = file_name;
    file_path1 = "../";
    file_path2 = "../";
    preprocess();
}

void Preprocess::include_file(string file_name) {
    file_reader.push_file(std::move(file_name));
}//载入新的文件
string Preprocess::replace(const string &new_str, string &old_str,const string &replace_str) {
    string::iterator now = old_str.begin(),end=old_str.end();
    string::iterator pre =now;
    string::iterator word_begin=now,word_end=now+1;
    unsigned int flag=0;
    while (now!=end){
        if(((*now>='a' && *now<='z') || (*now>='A' && *now<='Z')|| *now=='_') && flag==0){
            word_begin=now;
            word_end=word_begin+1;
            flag=1;
        } else if(((*now>='a' && *now<='z') || (*now>='A' && *now<='Z') ||
                   (*now>='0' && *now<='9') || *now=='_') && flag==1){
            if(word_end!=end){
                word_end++;
            }
        } else if(*now=='\"' && *pre!='\\'){
            if(flag==4){
                flag=0;
            } else if(flag==0){
                flag=4;
            }
        } else{
            if(flag!=4){
                flag=0;
            }
            if(replace_str==string(word_begin,word_end)){
                old_str.replace(word_begin,word_end,new_str);
                word_begin=word_end;
                word_end = now;
            }
        }
        pre=now;
        now++;
    }
    if(replace_str==string(word_begin,end)){
        old_str.replace(word_begin,word_end,new_str);
    }
    return old_str;
}
string Preprocess::replace(string &id,vector<string>& param_list) {
    string new_string,old_string;
    old_string = define_content[id];
    string::iterator start=old_string.begin(),end=old_string.end();
    string::iterator now=start;
    if(!param_list.empty()) {
        for (int i=0;i<param_list.size();i++) {
            new_string = replace(param_list[i],old_string,define_param[id][i]);
        }
    } else{
        new_string = old_string;
    }
//    out_string(new_string);
    return new_string;
}//替换宏定义未完成

string Preprocess::define(string::iterator &iter,const string::iterator &end, bool& define_not_end, const string& define_left
                          ,bool& define_first_time) {
    string::iterator now=iter;
    jmp_space(now);
    if(define_first_time){
        define_not_end = false;
        define_first_time=false;
        string::iterator head=now,tail=now;
        string define_first,define_second;
        vector<string> define_list;
        bool hasparam=false;
        while (*tail!=' ' && tail!=end && *tail!='('&& *tail!='\n'){//define左值
            define_first.push_back(*tail);
            tail++;
        }
        if(tail==end){
            define_content[define_first] = "";
        } else if(*tail=='('){
            define_first.push_back(*tail);
            hasparam=true;
            tail++;
        } else{
            tail++;
        }
        now=tail;
        iter = now;
//        cout << "define_first=" << define_first <<endl;
        if(hasparam){//define参数列表
            string element;
            while (now != end){
                if(*now==')' || *now==','){
                    define_list.emplace_back(element);
                    element.clear();
                    if(*now==')'){
                        now++;
                        break;
                    }
                } else if(*now==' '){

                } else{
                    element.push_back(*now);
                }
                now++;
            }
        }
        define_param[define_first] = define_list;
        jmp_space(now);
        while (now!=end){
            if(*now!='\\'){
                define_second.push_back(*now);
            } else{
                define_second.push_back('\n');
                define_not_end= true;
            }
            now++;
        }
//        cout << "define_second=" << define_second << endl;
        define_content[define_first] = define_second;
        iter = now;
        return define_first;
    } else{
        define_not_end = false;
        string define_second;
        while (now!=end){
            if(*now!='\\'){
                define_second.push_back(*now);
            } else{
                define_second.push_back('\n');
                define_not_end= true;
                define_content[define_left].append(define_second);
                iter = end;
                return define_left;
            }
            now++;
        }
        iter = now;
        return "";
    }
}//define操作未完成
void Preprocess::undef(string::iterator &iter,const string::iterator &end) {
    string::iterator now=iter;
    jmp_space(now);
    define_content.erase(string(now,end));
    define_param.erase(string(now,end));
    iter = end;
}//undef操作
void Preprocess::include(string::iterator &iter,const string::iterator &end){
    string new_file;
    string file_path;
    string::iterator now=iter;
    unsigned int flag=0;
    jmp_space(now);
    if(*now!='\"' && *now!='<'){
        throw runtime_error("Wrong Use of include");
    }
    now++;
    while (now!=end){
        if(*now=='>' || *now=='\"'){
            if(*now=='\"'){
                flag=1;
            } else if(*now=='>'){
                flag=2;
            }
            break;
        }
        new_file.push_back(*now);
        now++;
    }
    if(flag==1){
        file_path = file_path1+new_file;
    } else if(flag==2){
        file_path = file_path2+new_file;
    }
    if(include_list.find(file_path)==include_list.end()){
        include_file(file_path);
        include_list.insert(file_path);
    }
    iter = end;
}//include操作

void Preprocess::out_string(string str) {
    file.append(str);
}//保存到字符串中
void Preprocess::output() {
    ofstream a(file_name);
    a << file;
}//将file的内容输出至文件

bool Preprocess::is_comment(string::iterator &iter, const string::iterator &end,unsigned int &flag ) {
    string::iterator now=iter;
    string out;
    while(now!=end){
        if(flag==0 && *now=='/'){
            flag=1;
            out.append(string(iter,now));
        } else if(flag==1 && *now=='/'){
            flag=2;
            out.pop_back();
            break;
        } else if(flag==1 && *now=='*'){
            out.pop_back();
            flag=3;
        } else if(flag==3 && *now=='*'){
            flag=4;
        } else if(flag==4 && *now=='/'){
            flag=0;
        }
        now++;
    }
    if(flag==2 || flag==3){
        if(flag==2){
            flag=0;
        }
        iter = end;
        return true;
    } else{
        return false;
    }
}//判断是否是注释
bool Preprocess::is_define(string::iterator &iter,const string::iterator &end){
    string::iterator now=iter;
    string::iterator command_end=now;
    while (command_end!=end && *command_end!=' '){
        command_end++;
    }
    if(string(now,command_end)=="#define"){
        iter=command_end+1;
        return true;
    }else{
        return false;
    }
}//判断是否是define指令
bool Preprocess::is_undef(string::iterator &iter,const string::iterator &end){
    string::iterator now=iter;
    string::iterator command_end=now;
    while (command_end!=end && *command_end!=' '){
        command_end++;
    }
    if(string(now,command_end)=="#undef"){
        iter=command_end+1;
        return true;
    }else{
        return false;
    }
}//判断是否是undef指令
bool Preprocess::is_include(string::iterator &iter,const string::iterator &end){
    string::iterator now=iter;
    string::iterator command_end=now;
    while (command_end!=end && *command_end!=' '){
        command_end++;
    }
    if(string(now,command_end)=="#include"){
        iter=command_end+1;
        return true;
    }else{
        return false;
    }
}//判断是否是include指令
bool Preprocess::if_define(string& nowline,string::iterator &iter,const string::iterator &end){
    string::iterator now=iter;
    unsigned int flag=0;
    string id;
    vector<string> param_list;
    while (now!=end){
        if(((*now>='a' && *now<='z') || (*now>='A' && *now<='Z')|| *now=='_') && flag==0){
            id.push_back(*now);
            flag=1;
        } else if(((*now>='a' && *now<='z') || (*now>='A' && *now<='Z') ||
                (*now>='0' && *now<='9') || *now=='_') && flag==1){
            id.push_back(*now);
        } else if(*now=='(' && flag==1){
            id.push_back(*now);
            flag=2;
        } else if(*now=='\"'){
            flag=4;
        }else{
            if(define_content.find(id)!=define_content.end()){
                string param;
                if(define_param[id].size()>0){
                    while (*now!=')'){
                        jmp_space(now);
                        if(*now==','){
                            param_list.push_back(param);
                            param.clear();
                        } else{
                            param.push_back(*now);
                        }
                        now++;
                    }
                    if(*now==')'){
                        param_list.push_back(param);
                        now++;
                        param.clear();
                    }
                }
                if(define_param[id].size()!=param_list.size()){
                    throw runtime_error("Wrong Use of Define");
                }
                nowline.replace(iter,now,replace(id,param_list));
                cout << "define_nowline=" << nowline <<endl;
//                iter = now+1;
                flag=0;
                id.clear();
                return true;
            } else{
                iter = now+1;
                flag=0;
                id.clear();
                return false;
            }
        }
        now++;
    }
    return false;
}//是否为define替换内容

void Preprocess::set_path1(string &path) {
    this->file_path1 = path;
}
void Preprocess::set_path2(string &path) {
    this->file_path2 = path;
}

void Preprocess::preprocess() {
    string nowline;
    string define_left;
    unsigned int flag=0;
    bool define_not_end= false;
    bool define_first_time= false;
    while (file_reader.getfileline(nowline)){
        unsigned int com_flag=0;
        string::iterator start=nowline.begin(),end=nowline.end();
        string::iterator now = start;
        string::iterator a;
//        cout << "now_line=" << nowline << endl;
        while (now!=end){
            a=now;
            while (*now==' '){now++;}
            if(is_comment(now,end,flag)){

            }else if(*now=='#' || define_not_end){
                com_flag=1;
                out_string("\n");
                if(is_define(now,end) || define_not_end){
                    if(*a=='#' && !define_not_end){
                        define_first_time=true;
                    }
                    define_not_end = true;
                    define_left = define(now,end,define_not_end,define_left,define_first_time);
                } else if(is_undef(now,end)){
                    undef(now,end);
                } else if(is_include(now,end)){
                    include(now,end);
                } else{
                    throw runtime_error("Don't have the command");
                }
            } else{
                string::iterator out_begin=now;
                if(if_define(nowline,now,end) && (flag!=2 && flag!=3)){

                }
            }
        }
        if(nowline.size()==0){
            define_not_end= false;
            out_string("\n");
        } else if(com_flag==0){
            out_string(nowline);
            out_string("\n");
        }
    }
    file = to_string(file_reader.get_line()) + "\n" + file;
    output();
}
/*
 *
#define B C
#define A B
#define D(x,y) 7*x+3
#define E(x,y) {\
 x=-y;\
}\

int main()
{
int a=1;
int b=A;
a=D(2+3,4);
E(x,1);
}
//别错了
 */