//
// Created by wmj on 18-11-28.
//

#ifndef COMPLIE_ARRTIBUTE_H
#define COMPLIE_ARRTIBUTE_H

#include <vector>
#include "../word_analysis/TokenList.h"

#define ATTR_BUILDER [](const Token&t)->void*
#define ATTR [](std::vector<void*>& v)->void*
#define ITEM_V(x) ((Item*)v[(x)])->second
#define NONE 0

using AttrBuilder = void* (*)(const Token&);
using Attribute = void* (*)(std::vector<void*>&);

const AttrBuilder nothing_builder = [](const Token&)->void* {return nullptr;};
const Attribute nothing_attr = [](std::vector<void*>& v)->void* {return nullptr;};

const Attribute pass_attr = ATTR{return v[0];};

void* default_builder(const Token&);

void* attr_builder_if();
void* attr_builder_else();
void* attr_endif(std::vector<void*>& v);

void* attr_builder_while();
void* attr_builder_while_do();
void* attr_endwhile(std::vector<void*>& v);

void* attr_stmt_pos();

void* attr_builder_for_init();
void* attr_builder_for_cond();
void* attr_builder_for_inc();
void* attr_endfor(std::vector<void*>& v);


#endif //COMPLIE_ARRTIBUTE_H
