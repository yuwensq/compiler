#include "Unit.h"
#include "Type.h"
extern FILE *yyout;

void Unit::insertFunc(Function *f)
{
    func_list.push_back(f);
}

void Unit::removeFunc(Function *func)
{
    func_list.erase(std::find(func_list.begin(), func_list.end(), func));
}

void Unit::output() const
{
    // 先打印全局变量
    for (auto var : global_vars)
    {
        IdentifierSymbolEntry *se = (IdentifierSymbolEntry *)var;
        fprintf(yyout, "%s = global %s %d, align 4\n", se->toStr().c_str(), se->getType()->toStr().c_str(), se->getValue());
    }
    // 再打印函数
    for (auto &func : func_list)
        func->output();
    // 打印declare
    for (auto se : declare_list)
    {
        std::string type = ((FunctionType *)se->getType())->toStr();
        std::string name = se->toStr();
        std::string retType = type.substr(0, type.find("("));
        std::string fparams = type.substr(type.find("("));
        fprintf(yyout, "declare %s %s%s\n", retType.c_str(), name.c_str(), fparams.c_str());
    }
}

Unit::~Unit()
{
    for (auto &func : func_list)
        delete func;
}
