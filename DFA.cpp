//
// Created by wmj on 18-11-1.
//

#include "DFA.h"
using namespace std;
bool DFA::insert(const std::string& s) {
    Node* cursor = root;
    bool inserted = false;
    for (char c: s) {
        if (cursor->trans.find(c) == cursor->trans.end()) {
            Node* new_n = new Node();
            node_list.push_back(new_n);
            cursor->trans.insert(make_pair(c, new_n));
            inserted = true;
        }
        cursor = cursor->trans[c];
    }
    cursor->final = true;
    return inserted;
}

bool DFA::match(const std::string& s) {
    Node* cursor = root;
    for (char c: s) {
        if (cursor->trans.find(c) == cursor->trans.end())
            return false;
        cursor = cursor->trans[c];
    }
    return cursor->final;
}

DFA::DFA(std::vector<string> init_list) {
    root = new Node();
    node_list.push_back(root);
    for (const string& s: init_list)
        insert(s);
}

DFA::DFA() {
    root = new Node();
    node_list.push_back(root);
}

DFA::~DFA() {
    for (Node* n: node_list)
        delete n;
}

bool DFA::match(std::string::iterator &iter, const std::string::iterator &end, bool (*func)(int)) {
    string::iterator start = iter;
    Node* cursor = root;
    while (iter != end && (!func || func(*iter))) {
        if (cursor->trans.find(*iter) == cursor->trans.end())
            break;
        cursor = cursor->trans[*iter];
        iter++;
    }
    if (cursor->final)
        return true;
    iter = start;
    return false;
}

bool DFA::insert(std::string::iterator &iter, const std::string::iterator &end, bool (*func)(int)) {
    Node* cursor = root;
    bool inserted = false;
    while (iter != end && (!func || func(*iter))) {
        if (cursor->trans.find(*iter) == cursor->trans.end()) {
            Node* new_n = new Node();
            node_list.push_back(new_n);
            cursor->trans.insert(make_pair(*iter, new_n));
            inserted = true;
        }
        cursor = cursor->trans[*iter];
        iter++;
    }
    cursor->final = true;
    return inserted;
}
