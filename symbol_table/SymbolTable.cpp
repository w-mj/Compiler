//
// Created by wmj on 18-11-29.
//

#include "SymbolTable.h"

using namespace std;
SymbolTable::SymbolTable()
{
    constants = make_shared<Table>({CONSTANT});
    externals = make_shared<Table>({GLOBAL});
    identifiers = make_shared<Table>({GLOBAL});
    types = make_shared<Table>({GLOBAL});
    globals = identifiers;
}


std::shared_ptr<SymbolTable::Table>
SymbolTable::make_table(const std::shared_ptr<SymbolTable::Table> &base, uint level) {
    auto table = make_shared<Table>();
    table->level = level;
    table->previous = base;
    return table;
}


std::shared_ptr<SymbolTable::Symbol>
SymbolTable::install(const std::string &name, std::shared_ptr<SymbolTable::Table> tpp, uint level) {
    auto tp = tpp;
    auto symbol = make_shared<Symbol>();
    if (tp->level < level)
        tp = tpp = make_table(tp, level);
    symbol->name = name;
    symbol->scope = level;
    tp->table.emplace(name, tp->all_symbol.size());
    tp->all_symbol.push_back(symbol);
    return symbol;
}
