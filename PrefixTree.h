//
// Created by wmj on 18-11-1.
//

#ifndef COMPLIE_PrefixTree_H
#define COMPLIE_PrefixTree_H

#include <map>
#include <string>
#include <vector>

class PrefixTree {
private:
    struct Node {
        bool final = false;
        std::map<char, Node*> trans;
    };
    Node* root;
    std::vector<Node*> node_list;
public:
    PrefixTree();
    PrefixTree(std::vector<std::string> init_list);
    // insert a path into PrefixTree, return false for the path is already exists.
    bool insert(const std::string& s);
    bool insert(std::string::iterator& iter, const std::string::iterator& end, bool (*func)(int) = nullptr);
    bool match(const std::string& s);
    bool match(std::string::iterator& iter, const std::string::iterator& end, bool (*func)(int) = nullptr);
    ~PrefixTree();
};


#endif //COMPLIE_PrefixTree_H
