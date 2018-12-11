//
// Created by wmj on 18-11-28.
//

#include "Attribute.h"
#include "../word_analysis/WordAnalysis.h"
#include "../symbol_table/SymbolTable.h"

using C_GramData = Token;
using namespace std;
void* default_builder(const Token& tk) {
    if (tk.first == TOKEN_ID)
        return new string(SL.get_identification(tk.second));
    if (tk.first == TOKEN_BOUND) {
        string b = SL.get_bound(tk.second);
        if (b == "{")
            ST.in();
        else if (b == "}")
            ST.leave();
    }
    return new C_GramData( tk );
}