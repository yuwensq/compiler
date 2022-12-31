#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include <iostream>
#include <stack>
#include "Operand.h"
#include "Type.h"

class SymbolEntry;
class Unit;
class Function;
class BasicBlock;
class Instruction;
class IRBuilder;

class Node
{
private:
    static int counter;
    int seq;
    Node *next;

protected:
    std::vector<Instruction *> true_list;
    std::vector<Instruction *> false_list;
    static IRBuilder *builder;
    void backPatch(std::vector<Instruction *> &list, BasicBlock *bb);
    std::vector<Instruction *> merge(std::vector<Instruction *> &list1, std::vector<Instruction *> &list2);

public:
    Node();
    int getSeq() const { return seq; };
    static void setIRBuilder(IRBuilder *ib) { builder = ib; };
    virtual void output(int level) = 0;
    virtual void typeCheck() = 0;
    virtual void genCode() = 0;
    std::vector<Instruction *> &trueList() { return true_list; }
    std::vector<Instruction *> &falseList() { return false_list; }
    void setNext(Node *node);
    Node *getNext() { return next; }
};

class ExprNode : public Node
{
protected:
    bool isCond;
    SymbolEntry *symbolEntry;
    Operand *dst; // The result of the subtree is stored into dst.
    Type *type;

public:
    ExprNode(SymbolEntry *symbolEntry) : symbolEntry(symbolEntry), isCond(false){};
    Operand *getOperand() { return dst; };
    SymbolEntry *getSymPtr() { return symbolEntry; };
    bool isConde() const { return isCond; };
    void setIsCond(bool isCond) { this->isCond = isCond; };
    void output(int level);
    void genCode(){};
    void typeCheck(){};
    virtual int getValue() { return -1; };
    virtual Type *getType() { return type; };
};

class BinaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr1, *expr2;

public:
    enum
    {
        ADD,
        SUB,
        MUL,
        DIV,
        MOD,
        AND,
        OR,
        LESS,
        LESSEQUAL,
        GREATER,
        GREATEREQUAL,
        EQUAL,
        NOTEQUAL
    };
    BinaryExpr(SymbolEntry *se, int op, ExprNode *expr1, ExprNode *expr2);
    void output(int level);
    int getValue();
    void typeCheck();
    void genCode();
};

class UnaryExpr : public ExprNode
{
private:
    int op;
    ExprNode *expr;

public:
    enum
    {
        NOT,
        SUB
    };
    UnaryExpr(SymbolEntry *se, int op, ExprNode *expr);
    void output(int level);
    int getValue();
    void typeCheck();
    void genCode();
};

class CallExpr : public ExprNode
{
private:
    ExprNode *param;

public:
    CallExpr(SymbolEntry *se, ExprNode *param = nullptr);
    void output(int level);
    void typeCheck();
    void genCode();
};

class Constant : public ExprNode
{
public:
    Constant(SymbolEntry *se) : ExprNode(se)
    {
        dst = new Operand(se);
        type = TypeSystem::intType;
    };
    void output(int level);
    void typeCheck();
    void genCode();
    int getValue();
};

class Id : public ExprNode
{
private:
    ExprNode *index;

public:
    Id(SymbolEntry *se, ExprNode *index = nullptr) : ExprNode(se), index(index)
    {
        this->type = se->getType();
        if (se->getType()->isArray())
        {
            std::vector<int> indexs = ((ArrayType *)se->getType())->getIndexs();
            SymbolEntry *temp;
            if (index == nullptr) // 索引为空，引用数组指针
            {
                indexs.erase(indexs.begin());
                this->type = new PointerType(new ArrayType(indexs));
                temp = new TemporarySymbolEntry(this->type, SymbolTable::getLabel());
            }
            else
            { // 索引不为空，应该引用的是数组中的某个元素
                this->type = TypeSystem::intType;
                temp = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            }
            dst = new Operand(temp);
        }
        else if (se->getType()->isPtr())
        {
            SymbolEntry *temp;
            if (index == nullptr)
            {
                this->type = se->getType();
                temp = new TemporarySymbolEntry(se->getType(), SymbolTable::getLabel());
            }
            else
            {
                this->type = TypeSystem::intType;
                temp = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            }
            dst = new Operand(temp);
        }
        else
        {
            this->type = TypeSystem::intType;
            SymbolEntry *temp = new TemporarySymbolEntry(TypeSystem::intType, SymbolTable::getLabel());
            dst = new Operand(temp);
        }
    };
    void output(int level);
    void typeCheck();
    void genCode();
    ExprNode *getIndex() { return index; };
    int getValue();
};

// 这个用来bool转int和int转bool
class ImplictCastExpr : public ExprNode
{
private:
    ExprNode *expr;
    bool b2i; // true为bool转int，false为int转bool

public:
    ImplictCastExpr(ExprNode *expr, bool b2i = false) : ExprNode(nullptr), expr(expr), b2i(b2i)
    {
        type = b2i ? TypeSystem::intType : TypeSystem::boolType;
        dst = new Operand(new TemporarySymbolEntry(type, SymbolTable::getLabel()));
        if (expr->isConde())
        {
            expr->setIsCond(false);
            this->isCond = true;
        }
    };
    void output(int level);
    void typeCheck(){};
    void genCode();
};

class StmtNode : public Node
{
private:
    int kind;

protected:
    enum
    {
        IF,
        IFELSE,
        WHILE,
        COMPOUND,
        RETURN
    };

public:
    StmtNode(int kind = -1) : kind(kind){};
    bool isIf() const { return kind == IF; };
    void typeCheck(){};
    void genCode(){};
};

class CompoundStmt : public StmtNode
{
private:
    StmtNode *stmt;

public:
    CompoundStmt(StmtNode *stmt = nullptr) : stmt(stmt){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class SeqNode : public StmtNode
{
private:
    StmtNode *stmt1, *stmt2;

public:
    SeqNode(StmtNode *stmt1, StmtNode *stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class DeclStmt : public StmtNode
{
private:
    Id *id;
    ExprNode *expr;
    ExprNode **exprArray; // 当Id是个数组的时候，这个东西用来存数据的初始值吧

public:
    DeclStmt(Id *id, ExprNode *expr = nullptr) : id(id), expr(expr) { this->exprArray = nullptr; };
    void output(int level);
    void typeCheck();
    void genCode();
    void setInitArray(ExprNode **exprArray) { this->exprArray = exprArray; };
    Id *getId() { return id; };
};

class BlankStmt : public StmtNode
{
public:
    BlankStmt(){};
    void output(int level);
    void typeCheck(){};
    void genCode();
};

class IfStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;

public:
    IfStmt(ExprNode *cond, StmtNode *thenStmt);
    void output(int level);
    void typeCheck();
    void genCode();
};

class IfElseStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *thenStmt;
    StmtNode *elseStmt;

public:
    IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt);
    void output(int level);
    void typeCheck();
    void genCode();
};

class WhileStmt : public StmtNode
{
private:
    ExprNode *cond;
    StmtNode *stmt;
    BasicBlock *cond_bb;
    BasicBlock *end_bb;

public:
    WhileStmt(ExprNode *cond, StmtNode *stmt = nullptr);
    void setStmt(StmtNode *stmt) { this->stmt = stmt; };
    void output(int level);
    void typeCheck();
    void genCode();
    BasicBlock *get_cond_bb() { return this->cond_bb; };
    BasicBlock *get_end_bb() { return this->end_bb; };
};

class BreakStmt : public StmtNode
{
private:
    StmtNode *whileStmt;

public:
    BreakStmt(StmtNode *whileStmt);
    void output(int level);
    void typeCheck(){};
    void genCode();
};

class ContinueStmt : public StmtNode
{
private:
    StmtNode *whileStmt;

public:
    ContinueStmt(StmtNode *whileStmt);
    void output(int level);
    void typeCheck(){};
    void genCode();
};

class ReturnStmt : public StmtNode
{
private:
    ExprNode *retValue;

public:
    ReturnStmt(ExprNode *retValue = nullptr, Type *funcRetType = nullptr);
    void output(int level);
    void typeCheck();
    void genCode();
};

class AssignStmt : public StmtNode
{
private:
    ExprNode *lval;
    ExprNode *expr;

public:
    AssignStmt(ExprNode *lval, ExprNode *expr);
    void output(int level);
    void typeCheck();
    void genCode();
};

class ExprStmt : public StmtNode
{
private:
    ExprNode *expr;

public:
    ExprStmt(ExprNode *expr) : expr(expr){};
    void output(int level);
    void typeCheck();
    void genCode();
};

class FunctionDef : public StmtNode
{
private:
    SymbolEntry *se;
    DeclStmt *decl;
    StmtNode *stmt;

public:
    FunctionDef(SymbolEntry *se, DeclStmt *decl, StmtNode *stmt) : se(se), decl(decl), stmt(stmt){};
    void output(int level);
    void typeCheck();
    void genCode();
    SymbolEntry *getSymbolEntry() { return se; };
};

class Ast
{
private:
    Node *root;

public:
    Ast() { root = nullptr; }
    void setRoot(Node *n) { root = n; }
    void output();
    void typeCheck();
    void genCode(Unit *unit);
};

#endif
