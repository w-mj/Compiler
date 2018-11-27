//
// Created by wmj on 18-11-27.
//

#include "Grammar.h"
#include "../Utility.h"


Generators Grammar::Basic_Exp() {

    Generators gen;
    gen.set_terminators({"+", "-", "*", "/", "(", ")", "i"});
    gen.set_non_terminators({"E", "T", "F"});
    gen.set_start("E");
    gen.add("E") << "E + T" | "E - T" | "T";
    gen.add("T") << "T * F" | "T / F" | "F";
    gen.add("F") << "( E )" | "i";

    return gen;
}
