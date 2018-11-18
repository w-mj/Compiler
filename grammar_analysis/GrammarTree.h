//
// Created by wmj on 18-11-18.
//

#ifndef COMPLIE_GRAMMARTREE_H
#define COMPLIE_GRAMMARTREE_H

#include <string>
#include <vector>
#include <fstream>

struct Node {
    std::string text;
    std::vector<Node*> children;
    Node* parent = nullptr;
    Node(std::string s);
    void json(std::ofstream &of);
    void json();

};

void delete_tree(Node* node);

class GrammarTree {
    Node* root;
public:
    explicit GrammarTree(Node* node);
    ~GrammarTree();

};


#endif //COMPLIE_GRAMMARTREE_H
