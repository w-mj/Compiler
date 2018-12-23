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
    ofstream fhtml;
    fhtml.open("grammer_tree.html");
    fhtml <<
    "<!DOCTYPE html>\n"
    "<html lang=\"en\">\n"
    "<head>\n"
    "    <meta charset=\"utf-8\">\n"
    "\n"
    "    <title>Collapsible Tree Example</title>\n"
    "\n"
    "    <style>\n"
    "\n"
    "\t.node circle {\n"
    "\t  fill: #fff;\n"
    "\t  stroke: steelblue;\n"
    "\t  stroke-width: 3px;\n"
    "\t}\n"
    "\n"
    "\t.node text { font: 12px sans-serif; }\n"
    "\n"
    "\t.link {\n"
    "\t  fill: none;\n"
    "\t  stroke: #ccc;\n"
    "\t  stroke-width: 2px;\n"
    "\t}\n"
    "\n"
    "    </style>\n"
    "\n"
    "</head>\n"
    "\n"
    "<body>\n"
    "\n"
    "<!-- load the d3.js library -->\n"
    "<script src=\"http://d3js.org/d3.v3.min.js\"></script>\n"
    "\n"
    "<script>\n"
    "\n"
    "d3.json(\"grammar_tree.json\", function(error, treeData) {\n"
    "\n"
    "\n"
    "\t// ************** Generate the tree diagram\t *****************\n"
    "var margin = {top: 40, right: 40, bottom: 20, left: 40},\n"
    "\twidth = 2000 - margin.right - margin.left,\n"
    "\theight = 2000 - margin.top - margin.bottom;\n"
    "\n"
    "var i = 0;\n"
    "\n"
    "var tree = d3.layout.tree()\n"
    "\t.size([height, width]);\n"
    "\n"
    "tree.separation(function(a, b) { return (a.parent == b.parent ? 1 : 2) / a.depth; });\n"
    "\n"
    "var diagonal = d3.svg.diagonal()\n"
    "\t.projection(function(d) { return [d.x, d.y]; });\n"
    "\n"
    "var svg = d3.select(\"body\").append(\"svg\")\n"
    "\t.attr(\"width\", width + margin.right + margin.left)\n"
    "\t.attr(\"height\", height + margin.top + margin.bottom)\n"
    "  .append(\"g\")\n"
    "\t.attr(\"transform\", \"translate(\" + margin.left + \",\" + margin.top + \")\");\n"
    "\n"
    "root = treeData;\n"
    "\n"
    "update(root);\n"
    "\n"
    "function update(source) {\n"
    "\n"
    "  // Compute the new tree layout.\n"
    "  var nodes = tree.nodes(root).reverse(),\n"
    "\t  links = tree.links(nodes);\n"
    "\n"
    "  // Normalize for fixed-depth.\n"
    "  // nodes.forEach(function(d) { d.y = d.depth * 80; });\n"
    "\n"
    "  // Declare the nodes…\n"
    "  var node = svg.selectAll(\"g.node\")\n"
    "\t  .data(nodes, function(d) { return d.id || (d.id = ++i); });\n"
    "\n"
    "  // Enter the nodes.\n"
    "  var nodeEnter = node.enter().append(\"g\")\n"
    "\t  .attr(\"class\", \"node\")\n"
    "\t  .attr(\"transform\", function(d) {\n"
    "\t\t  return \"translate(\" + d.x + \",\" + d.y + \")\"; });\n"
    "\n"
    "  nodeEnter.append(\"circle\")\n"
    "\t  .attr(\"r\", 10)\n"
    "\t  .style(\"fill\", \"#fff\");\n"
    "\n"
    "  nodeEnter.append(\"text\")\n"
    "\t  .attr(\"y\", function(d) {\n"
    "\t\t  return d.children || d._children ? -18 : 18; })\n"
    "\t  .attr(\"dy\", \".35em\")\n"
    "\t  .attr(\"text-anchor\", \"middle\")\n"
    "\t  .text(function(d) { return d.name; })\n"
    "\t  .style(\"fill-opacity\", 1);\n"
    "\n"
    "  // Declare the links…\n"
    "  var link = svg.selectAll(\"path.link\")\n"
    "\t  .data(links, function(d) { return d.target.id; });\n"
    "\n"
    "  // Enter the links.\n"
    "  link.enter().insert(\"path\", \"g\")\n"
    "\t  .attr(\"class\", \"link\")\n"
    "\t  .attr(\"d\", diagonal);\n"
    "\n"
    "}\n"
    "});\n"
    "\n"
    "\n"
    "\n"
    "\n"
    "</script>\n"
    "\n"
    "</body>\n"
    "</html>";
    fhtml.close();
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
