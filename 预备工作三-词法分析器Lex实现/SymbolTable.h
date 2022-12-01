#include <vector>
#include <string>

struct SymbolAttributes {

};

struct SymbolItem {
    std::string lexemer;
    SymbolAttributes *attibutes;
};

struct SymbolTable {
    SymbolTable *prev;     
    int cnt;
    SymbolItem symbols[100];
    SymbolTable() {
        cnt = 0;
        prev = nullptr;
    }
};

SymbolItem *look_for(SymbolTable*, std::string);

SymbolItem *insert(SymbolTable*, std::string);