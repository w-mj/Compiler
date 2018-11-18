//
// Created by wmj on 18-11-18.
//

#include "GrammarTree.h"
using namespace std;
GrammarTree::GrammarTree(Node *node): root(node){

}

GrammarTree::~GrammarTree() {
    delete_tree(root);
}

void delete_tree(Node *node) {
    for (Node* n: node->children)
        delete_tree(n);
    delete node;
}



Node::Node(const std::string s): text(s) {

}

void Node::json() {
    ofstream file;
    file.open("grammar_tree.json");
    // file << "JSON_DATA = ";
    json(file);
    file.close();
}

void Node::json(std::ofstream &of) {
    of << R"({"name":")" << text << "\"\n";
    if (!children.empty()) {
        of << ",\n\"children\": [\n";
        for (size_t i = 0; i < children.size(); i++) {
            children[i]->json(of);
            if (i != children.size() - 1)
                of << ",\n";
        }
        of << "]\n";
    }
    of << "}\n";
}
