//
// Created by wmj on 18-11-28.
//

#include "Attribute.h"
#include "../word_analysis/WordAnalysis.h"

using C_GramData = Token;
using namespace std;
void* default_builder(const Token& tk) {
    if (tk.first == TOKEN_ID)
        return new string(SL.get_identification(tk.second));
    return new C_GramData( tk );
}