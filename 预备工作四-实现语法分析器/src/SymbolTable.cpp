#include "SymbolTable.h"
#include <iostream>
#include <sstream>
#include "Type.h"

SymbolEntry::SymbolEntry(Type *type, int kind)
{
    this->type = type;
    this->kind = kind;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, int value)
    : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    assert(type->isInt());
    this->value.type = Value::INT;
    this->value.intValue = value;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, float value)
    : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    assert(type->isFloat());
    this->value.type = Value::FLOAT;
    this->value.floatValue = value;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type, std::string value)
    : SymbolEntry(type, SymbolEntry::CONSTANT)
{
    assert(type->isString());
    this->strValue = value;
}

ConstantSymbolEntry::ConstantSymbolEntry(Type *type)
    : SymbolEntry(type, SymbolEntry::CONSTANT)
{
}

Value ConstantSymbolEntry::getValue() const
{
    return value;
}

std::string ConstantSymbolEntry::getStrValue() const
{
    assert(type->isString());
    return strValue;
}

std::string ConstantSymbolEntry::toStr()
{
    std::ostringstream buffer;
    if (type->isInt())
        buffer << value.intValue;
    else if (type->isFloat())
        buffer << value.floatValue;
    else if (type->isString())
        buffer << strValue;
    return buffer.str();
}

IdentifierSymbolEntry::IdentifierSymbolEntry(Type *type,
                                             std::string name,
                                             int scope)
    : SymbolEntry(type, SymbolEntry::VARIABLE), name(name)
{
    this->scope = scope;
    this->initial = false;
    if (type->isInt()) 
        this->value.type = Value::INT;
    else if (type->isFloat())
        this->value.type = Value::FLOAT;
}

void IdentifierSymbolEntry::setValue(Value value)
{
    if ((this->getType()->isInt() && ((IntType*)this->getType())->isConst()) \
    || (this->getType()->isFloat() && ((FloatType*)this->getType())->isConst()))
    {
        if (!initial)
        {
            this->value.asign(value);
            initial = true;
        }
        else
        {
            // 报错
        }
    }
    else
    {
        this->value.asign(value);
    }
}

std::string IdentifierSymbolEntry::toStr()
{
    return name;
}

TemporarySymbolEntry::TemporarySymbolEntry(Type *type, int label)
    : SymbolEntry(type, SymbolEntry::TEMPORARY)
{
    this->label = label;
}

std::string TemporarySymbolEntry::toStr()
{
    std::ostringstream buffer;
    buffer << "t" << label;
    return buffer.str();
}

SymbolTable::SymbolTable()
{
    prev = nullptr;
    level = 0;
}

SymbolTable::SymbolTable(SymbolTable *prev)
{
    this->prev = prev;
    this->level = prev->level + 1;
}

/*
    Description: lookup the symbol entry of an identifier in the symbol table
    Parameters:
        name: identifier name
    Return: pointer to the symbol entry of the identifier

    hint:
    1. The symbol table is a stack. The top of the stack contains symbol entries
   in the current scope.
    2. Search the entry in the current symbol table at first.
    3. If it's not in the current table, search it in previous ones(along the
   'prev' link).
    4. If you find the entry, return it.
    5. If you can't find it in all symbol tables, return nullptr.
*/
SymbolEntry *SymbolTable::lookup(std::string name, bool local) //local表示是否只在本地表查该标识符
{
    SymbolTable *table = this;
    while (table != nullptr)
    {
        if (table->symbolTable.find(name) != table->symbolTable.end())
        {
            return table->symbolTable[name];
        }
        table = table->prev;
        if (local)
        {
            if (table == nullptr || table->getLevel() != 1)
                break;
        }
    }
    return nullptr;
}

// install the entry into current symbol table.
void SymbolTable::install(std::string name, SymbolEntry *entry)
{
    symbolTable[name] = entry;
}

int SymbolTable::counter = 0;
static SymbolTable t;
SymbolTable *identifiers = &t;
SymbolTable *globals = &t;
