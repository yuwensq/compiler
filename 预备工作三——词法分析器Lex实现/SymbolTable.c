#include "SymbolTable.h"
#include <iostream>

SymbolItem *look_for(SymbolTable *st, std::string lexemer) {
    SymbolTable *now_st = st;
    while (now_st != nullptr) {
        for (int i = 0; i < now_st->cnt; i++) {
            if (now_st->symbols[i].lexemer == lexemer) {
                return &now_st->symbols[i];
            }
        }
        now_st = now_st->prev;
    }
    return nullptr;
}

SymbolItem *insert(SymbolTable *st, std::string lexemer) {
    if (st == nullptr) {
        return nullptr;
    }
    SymbolItem si;
    si.lexemer = lexemer;
    st->symbols[st->cnt++] = si;
    return &st->symbols[st->cnt - 1];
}