#include <iostream>
#include <fstream>
#include <string>
#include "word_analysis/WordAnalysis.h"
using namespace std;

int main() {
    string fname = "../test.txt";
    ifstream file;
    file.open(fname);
    string line;
    WordAnalysis analyzer;
    string::iterator start;
    bool in_commit = false;
    while(getline(file, line)) {
        cout << "in line: " << line << endl;
        string::iterator iter = line.begin();
        while (iter != line.end()) {
            if (!in_commit && *iter == '/' && *(iter + 1) == '/')  // inline commit
                break;
            if ((*iter == '/' && *(iter + 1) == '*'))  // block commit
                in_commit = true;

            if (in_commit) {
                while (iter != line.end() && !(*iter == '*' && *(iter + 1) == '/'))
                    iter++;
                if (*iter == '*' && *(iter + 1) == '/') {
                    in_commit = false;
                    iter += 2;
                }
                continue;
            }
            try {
                if (isspace(*iter)) {
                    iter++;
                    continue;  // skip space characters
                }
                start = iter;
                if (isalpha(*iter)) {
                    if (analyzer.process_key(iter, line.end())) {
                        cout << "  " << analyzer.token2str(-1) << endl;
                    } else if (analyzer.process_id(iter, line.end())) {
                        cout << "  " << analyzer.token2str(-1) << endl;
                    } else
                        iter++;
                } else if (isdigit(*iter) || (*iter == '.' && isdigit(*(iter+1))) ) {
                    analyzer.process_constant(iter, line.end());
                    cout << "  " << analyzer.token2str(-1) << endl;
                } else if (*iter == '"') {
                    analyzer.process_str(iter, line.end());
                    cout << "  " << analyzer.token2str(-1) << endl;
                } else if (*iter == '\'') {
                    analyzer.process_char(iter, line.end());
                    cout << "  " << analyzer.token2str(-1) << endl;
                } else {
                    if (analyzer.process_bound(iter, line.end()))
                        cout << "  " << analyzer.token2str(-1) << endl;
                    else
                        iter++;
                }
            } catch (runtime_error &e) {
                cout << endl << "ERROR:" << endl << line << endl;
                for (int i = 0; i < start - line.begin(); i++)
                    cout << ' ';
                for (int i = 0; i < iter - start; i++)
                    cout << '~';
                cout << "^" << endl;
                cout << e.what() << endl;
                exit(100);
            }
        }
    }
    return 0;
}