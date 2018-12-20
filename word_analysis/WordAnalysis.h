//
// Created by wmj on 18-11-1.
//

#ifndef COMPLIE_WORDANALYSIS_H
#define COMPLIE_WORDANALYSIS_H

#include <vector>
#include <string>
#include <map>
#include "PrefixTree.h"
#include "NumericDFA.h"
#include "TokenList.h"

#define WA WordAnalysis::getInstance()
#define SL WA.get_tokenList()

enum {
    kchar=0, kdouble, kenum, kfloat, kint, klong, kshort, ksigned, kstruct, kunion, kunsigned, kvoid, kfor, kdo,
    kwhile, kbreak, kcontinue, kif, kelse, kgoto, kswitch, kcase, kdefault, kreturn, kauto, kextern, kregister,
    kstatic, kconst, ksizeof, ktypedef, kvolatile
};

enum {
    pinc=0, pdec, plp, prp, plb, prb, pdot, ppointer, pplus, pminus, pnot, pinv, pband, pmultply, pdivide, pmod,
    psl, psr, plt, ppe, pgt, pge, peq, pne, pxor, pbor, pand, por, pqes, pcolon, passign, paddass, pminusass,
    pmultplyass, pdivass, pmodass, pslass, psrass, pandass, pxorass, poras, pcomma, psimi, plc, prc
};

class WordAnalysis {
private:

    PrefixTree key_tree;
    PrefixTree id_tree;
    PrefixTree bound_tree;

    TokenList tokenList;

    std::map<std::string, int> key_map, bound_map;

    int getEscape(std::string::iterator s, std::string::iterator e);
    explicit WordAnalysis();

public:
    WordAnalysis(WordAnalysis&) = delete;
    WordAnalysis& operator=(WordAnalysis&) = delete;

    static WordAnalysis& getInstance() {
        static WordAnalysis ins;
        return ins;
    }

    std::vector<std::string> key;
    std::vector<std::string> bound;

    const std::map<std::string, int>& get_key_map();
    const std::map<std::string, int>& get_bound_map();


    bool process_key(std::string::iterator& iter, const std::string::iterator& end);
    bool process_id(std::string::iterator& iter, const std::string::iterator& end);
    bool process_str(std::string::iterator& iter, const std::string::iterator& end);
    bool process_char(std::string::iterator& iter, const std::string::iterator& end);
    bool process_constant(std::string::iterator& iter, const std::string::iterator &end);
    bool process_bound(std::string::iterator& iter, const std::string::iterator &end);

    void process_file(std::ifstream &file, bool print=false);
    TokenList get_tokenList() const;  //TODO: 引用TokenList

};

std::ostream& print_token(std::ostream& out, const TokenList& tk, long s, long e);


#endif //COMPLIE_WORDANALYSIS_H
