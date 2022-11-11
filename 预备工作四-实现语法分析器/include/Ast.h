#ifndef __AST_H__
#define __AST_H__

#include <fstream>
#include "Value.h"

class SymbolEntry;

class Node {
   private:
    static int counter;
    int seq;
    Node* next;

   public:
    Node();
    int getSeq() const { return seq; };
    virtual void output(int level) = 0;
    void setNext(Node* node);
    Node* getNext() { return next; }
};

class ExprNode : public Node {
   private:
    int kind;

   protected:
    SymbolEntry* symbolEntry;
    enum { EXPR };

   public:
    ExprNode(SymbolEntry* symbolEntry, int kind = EXPR)
        : kind(kind), symbolEntry(symbolEntry){};
    void output(int level);
    virtual Value getValue() { Value value(Value::INT, 0); return value; };
    bool isExpr() const { return kind == EXPR; };
    SymbolEntry* getSymbolEntry() const { return symbolEntry; };

};

class BinaryExpr : public ExprNode {
   private:
    int op;
    ExprNode *expr1, *expr2;

   public:
    enum {
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
    BinaryExpr(SymbolEntry* se, int op, ExprNode* expr1, ExprNode* expr2)
        : ExprNode(se), op(op), expr1(expr1), expr2(expr2){};
    void output(int level);
    Value getValue();
};

class UnaryExpr : public ExprNode {
   private:
    int op;
    ExprNode* expr;

   public:
    enum { NOT, SUB };
    UnaryExpr(SymbolEntry* se, int op, ExprNode* expr)
        : ExprNode(se), op(op), expr(expr){};
    void output(int level);
    Value getValue();
};

class CallExpr : public ExprNode {
   private:
    ExprNode* param;

   public:
    CallExpr(SymbolEntry* se, ExprNode* param = nullptr)
        : ExprNode(se), param(param){};
    void output(int level);
};

class Constant : public ExprNode {
   public:
    Constant(SymbolEntry* se) : ExprNode(se){};
    void output(int level);
    Value getValue();
};

class Id : public ExprNode {
   private:
    ExprNode* arrIdx;

   public:
    Id(SymbolEntry* se, ExprNode* arrIdx = nullptr)
        : ExprNode(se), arrIdx(arrIdx){};
    void output(int level);
    SymbolEntry* getSymbolEntry() { return symbolEntry; };
    Value getValue();
};

class StmtNode : public Node {};

class CompoundStmt : public StmtNode {
   private:
    StmtNode* stmt;

   public:
    CompoundStmt(StmtNode* stmt = nullptr) : stmt(stmt){};
    void output(int level);
};

class SeqNode : public StmtNode {
   private:
    StmtNode *stmt1, *stmt2;

   public:
    SeqNode(StmtNode* stmt1, StmtNode* stmt2) : stmt1(stmt1), stmt2(stmt2){};
    void output(int level);
};

class DeclStmt : public StmtNode {
   private:
    Id* id;
    ExprNode* expr;

   public:
    DeclStmt(Id* id, ExprNode* expr = nullptr) : id(id), expr(expr){};
    void output(int level);
    Id* getId() { return id; };
};

class BlankStmt : public StmtNode {
   public:
    BlankStmt(){};
    void output(int level);
};

class IfStmt : public StmtNode {
   private:
    ExprNode* cond;
    StmtNode* thenStmt;

   public:
    IfStmt(ExprNode* cond, StmtNode* thenStmt)
        : cond(cond), thenStmt(thenStmt){};
    void output(int level);
};

class IfElseStmt : public StmtNode {
   private:
    ExprNode* cond;
    StmtNode* thenStmt;
    StmtNode* elseStmt;

   public:
    IfElseStmt(ExprNode* cond, StmtNode* thenStmt, StmtNode* elseStmt)
        : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt){};
    void output(int level);
};

class WhileStmt : public StmtNode {
   private:
    ExprNode* cond;
    StmtNode* stmt;

   public:
    WhileStmt(ExprNode* cond, StmtNode* stmt) : cond(cond), stmt(stmt){};
    void output(int level);
};

class BreakStmt : public StmtNode {
   public:
    BreakStmt(){};
    void output(int level);
};

class ContinueStmt : public StmtNode {
   public:
    ContinueStmt(){};
    void output(int level);
};

class ReturnStmt : public StmtNode {
   private:
    ExprNode* retValue;

   public:
    ReturnStmt(ExprNode* retValue = nullptr) : retValue(retValue){};
    void output(int level);
};

class AssignStmt : public StmtNode {
   private:
    ExprNode* lval;
    ExprNode* expr;

   public:
    AssignStmt(ExprNode* lval, ExprNode* expr) : lval(lval), expr(expr){};
    void output(int level);
};

class ExprStmt : public StmtNode {
   private:
    ExprNode* expr;

   public:
    ExprStmt(ExprNode* expr) : expr(expr){};
    void output(int level);
};

class FunctionDef : public StmtNode {
   private:
    SymbolEntry* se;
    DeclStmt* decl;
    StmtNode* stmt;

   public:
    FunctionDef(SymbolEntry* se, DeclStmt* decl, StmtNode* stmt)
        : se(se), decl(decl), stmt(stmt){};
    void output(int level);
};

class Ast {
   private:
    Node* root;

   public:
    Ast() { root = nullptr; }
    void setRoot(Node* n) { root = n; }
    void output();
};

#endif
