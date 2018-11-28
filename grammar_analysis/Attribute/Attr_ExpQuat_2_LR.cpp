//
// Created by wmj on 18-11-28.
//

#include "Attr_ExpQuat_2_LR.h"
#include "string"
#include "../../word_analysis/TokenList.h"

using namespace std;

string s_or_(const string& s) {
    if (s.empty())
        return "_";
    return s;
}

std::ostream &operator<<(std::ostream &out, const Quaternary &quat) {
    out << "(" << s_or_(quat.op) << ", " << s_or_(quat.num1) << ", " << s_or_(quat.num2) << ", " << s_or_(quat.target) << ")";
    return out;
}

int Attr_ExpQuat_2_LR::t_cnt = 0;


void *Attr_ExpQuat_2_LR::operator()(const std::vector<void*> &sub_tokens) {
    string op, num1, num2, target;
    // TODO: associate with global char table.
    Token* tkl[3];
    for (int i = 0; i < 3; i++)
        tkl[i] = (Token*)sub_tokens[i];
    if (tkl[0]->first == 'z')
        num1 = "t" + to_string(tkl[0]->second);
    else
        num1 = tokenList.get_token_str(*(Token*)sub_tokens[0]);
    if (tkl[2]->first == 'z')
        num2 = "t" + to_string(tkl[2]->second);
    else
        num2 = tokenList.get_token_str(*(Token*)sub_tokens[2]);
    target = "t" + to_string(t_cnt);
    op = tokenList.get_token_str(*(Token*)sub_tokens[1]);
    Quaternary q {op, num1, num2, target};
    cout << q << endl;
    return new Token('z', t_cnt++);
}

Attr_ExpQuat_2_LR::Attr_ExpQuat_2_LR(TokenList &tokenList): tokenList(tokenList){

}

void *Attr_ExpQuat_2_LR::get_data(TokenList::iterator &it) {
//    if (it->first == 'p' && !(it->second == '+' || it->second == '-' || it->second == '*' || it->second == '/'))
//        return nullptr;
    return new Token(it->first, it->second);
}

void *process_B::operator()(const std::vector<void *> &sub_tokens) {
    return sub_tokens[1];
}
