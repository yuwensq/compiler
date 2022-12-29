#include "Ast.h"
#include <stack>
#include <string>
#include <iostream>
#include "IRBuilder.h"
#include "Instruction.h"
#include "SymbolTable.h"
#include "Type.h"
#include "Unit.h"

#define now_bb (builder->getInsertBB())
#define now_func (builder->getInsertBB()->getParent())
extern Unit unit;

extern FILE *yyout;
int Node::counter = 0;
IRBuilder *Node::builder = nullptr;

// 构造函数之类的代码区

Node::Node()
{
    seq = counter++;
    next = nullptr;
}

void Node::setNext(Node *node)
{
    Node *n = this;
    while (n->getNext())
    {
        n = n->getNext();
    }
    n->next = node;
}

BinaryExpr::BinaryExpr(SymbolEntry *se, int op, ExprNode *expr1, ExprNode *expr2) : ExprNode(se), op(op), expr1(expr1), expr2(expr2)
{
    this->dst = new Operand(se);
    this->type = se->getType();
    std::string op_type;
    switch (op)
    {
    case ADD:
        op_type = "+";
        break;
    case SUB:
        op_type = "-";
        break;
    case MUL:
        op_type = "*";
        break;
    case DIV:
        op_type = "/";
        break;
    case MOD:
        op_type = "%";
        break;
    case AND:
        op_type = "&&";
        break;
    case OR:
        op_type = "||";
        break;
    case LESS:
        op_type = "<";
        break;
    case LESSEQUAL:
        op_type = "<=";
        break;
    case GREATER:
        op_type = ">";
        break;
    case GREATEREQUAL:
        op_type = ">=";
        break;
    case EQUAL:
        op_type = "==";
        break;
    case NOTEQUAL:
        op_type = "!=";
        break;
    }
    if (expr1->getType()->isVoid() || expr2->getType()->isVoid())
    {
        fprintf(stderr, "invalid operand of type \'void\' to binary \'opeartor%s\'\n", op_type.c_str());
    }
    // 前几个操作符是算术运算符，返回类型是int型，后面是逻辑运算符，返回类型是Bool型
    if (op >= BinaryExpr::AND && op <= BinaryExpr::NOTEQUAL)
    {
        // 对于AND和OR逻辑运算，如果操作数表达式为int型，需要进行隐式转换，将int型转为bool型。
        if (op == BinaryExpr::AND || op == BinaryExpr::OR)
        {
            if (expr1->getType()->isInt())
            {
                this->expr1 = new ImplictCastExpr(expr1);
            }
            if (expr2->getType()->isInt())
            {
                this->expr2 = new ImplictCastExpr(expr2);
            }
        }
        else
        {
            if (expr1->getType()->isBool())
            {
                this->expr1 = new ImplictCastExpr(expr1, true);
            }
            if (expr2->getType()->isBool())
            {
                this->expr2 = new ImplictCastExpr(expr2, true);
            }
        }
    }
}

int BinaryExpr::getValue()
{
    int value1 = expr1->getValue();
    int value2 = expr2->getValue();
    int value;
    switch (op)
    {
    case ADD:
        value = value1 + value2;
        break;
    case SUB:
        value = value1 - value2;
        break;
    case MUL:
        value = value1 * value2;
        break;
    case DIV:
        value = value1 / value2;
        break;
    case MOD:
        value = value1 % value2;
        break;
    case AND:
        value = value1 && value2;
        break;
    case OR:
        value = value1 || value2;
        break;
    case LESS:
        value = value1 < value2;
        break;
    case LESSEQUAL:
        value = value1 <= value2;
        break;
    case GREATER:
        value = value1 > value2;
        break;
    case GREATEREQUAL:
        value = value1 >= value2;
        break;
    case EQUAL:
        value = value1 == value2;
        break;
    case NOTEQUAL:
        value = value1 != value2;
        break;
    }
    return value;
}

UnaryExpr::UnaryExpr(SymbolEntry *se, int op, ExprNode *expr) : ExprNode(se), op(op), expr(expr)
{
    this->dst = new Operand(se);
    this->type = se->getType();
    std::string op_str;
    switch (op)
    {
    case NOT:
        op_str = "!";
        break;
    case SUB:
        op_str = "-";
        break;
    }
    if (expr->getType()->isVoid())
    {
        fprintf(stderr, "invalid operand of type \'void\' to unary \'opeartor%s\'\n", op_str.c_str());
    }
    if (op == UnaryExpr::NOT)
    {
        if (expr->getType()->isInt())
        {
            this->expr = new ImplictCastExpr(expr);
        }
    }
    else if (op == UnaryExpr::SUB)
    {
        if (expr->getType()->isBool())
        {
            this->expr = new ImplictCastExpr(expr, true);
        }
    }
}

int UnaryExpr::getValue()
{
    int value = expr->getValue();
    switch (op)
    {
    case NOT:
        value = (!value);
        break;
    case SUB:
        value = (-value);
        break;
    }
    return value;
}

int Constant::getValue()
{
    return ((ConstantSymbolEntry *)symbolEntry)->getValue();
}

int Id::getValue()
{
    return ((IdentifierSymbolEntry *)symbolEntry)->getValue();
}

CallExpr::CallExpr(SymbolEntry *se, ExprNode *param) : ExprNode(se), param(param)
{
    dst = nullptr;
    // 统计实参数量
    unsigned long int paramCnt = 0;
    ExprNode *temp = param;
    while (temp)
    {
        paramCnt++;
        temp = (ExprNode *)(temp->getNext());
    }
    // 由于存在函数重载的情况，这里我们提前将重载的函数通过符号表项的next指针连接，这里需要根据实参个数判断对应哪一个具体函数，找到其对应得符号表项
    SymbolEntry *s = se;
    Type *type;
    std::vector<Type *> FParams;
    while (s)
    {
        type = s->getType();
        FParams = ((FunctionType *)type)->getParamsType();
        if (paramCnt == FParams.size())
        {
            this->symbolEntry = s;
            break;
        }
        s = s->getNext();
    }
    if (symbolEntry)
    {
        // 如果函数返回值类型不为空，需要存储返回结果
        this->type = ((FunctionType *)type)->getRetType();
        if (this->type != TypeSystem::voidType)
        {
            SymbolEntry *se = new TemporarySymbolEntry(this->type, SymbolTable::getLabel());
            dst = new Operand(se);
        }
        ExprNode *temp = param;
        // 逐个比较形参列表和实参列表中每个参数的类型是否相同
        for (auto it = FParams.begin(); it != FParams.end(); it++)
        {
            if (temp == nullptr)
            {
                fprintf(stderr, "too few arguments to function %s %s\n", symbolEntry->toStr().c_str(), type->toStr().c_str());
                break;
            }
            // else if ((*it)->getKind() != temp->getType()->getKind())
            // {
            //     fprintf(stderr, "parameter's type %s can't convert to %s\n", temp->getType()->toStr().c_str(), (*it)->toStr().c_str());
            // }
            temp = (ExprNode *)(temp->getNext());
        }
        if (temp != nullptr)
        {
            fprintf(stderr, "too many arguments to function %s %s\n", symbolEntry->toStr().c_str(), type->toStr().c_str());
        }
    }
    else
    {
        fprintf(stderr, "function is undefined\n");
    }
    if (((IdentifierSymbolEntry *)se)->isSysy())
    {
        unit.insertDeclare(se);
    }
}

IfStmt::IfStmt(ExprNode *cond, StmtNode *thenStmt) : cond(cond), thenStmt(thenStmt)
{
    if (cond->getType()->isInt())
    {
        this->cond = new ImplictCastExpr(cond);
    }
}

IfElseStmt::IfElseStmt(ExprNode *cond, StmtNode *thenStmt, StmtNode *elseStmt) : cond(cond), thenStmt(thenStmt), elseStmt(elseStmt)
{
    if (cond->getType()->isInt())
    {
        this->cond = new ImplictCastExpr(cond);
    }
}

WhileStmt::WhileStmt(ExprNode *cond, StmtNode *stmt) : cond(cond), stmt(stmt)
{
    if (cond->getType()->isInt())
    {
        this->cond = new ImplictCastExpr(cond);
    }
}

BreakStmt::BreakStmt(StmtNode *whileStmt) : whileStmt(whileStmt)
{
    if (whileStmt == nullptr)
    {
        fprintf(stderr, "no while stmt for this break stmt\n");
    }
}

ContinueStmt::ContinueStmt(StmtNode *whileStmt) : whileStmt(whileStmt)
{
    if (whileStmt == nullptr)
    {
        fprintf(stderr, "no while stmt for this continue stmt\n");
    }
}

ReturnStmt::ReturnStmt(ExprNode *retValue, Type *funcRetType) : retValue(retValue)
{
    // 判断返回值和函数返回值是否一致
    Type *retType;
    if (retValue == nullptr)
    {
        retType = TypeSystem::voidType;
    }
    else
    {
        retType = retValue->getType();
    }
    if (retType->getKind() != funcRetType->getKind())
    {
        fprintf(stderr, "return type isn't equal to function type\n");
    }
}

AssignStmt::AssignStmt(ExprNode *lval, ExprNode *expr) : lval(lval), expr(expr)
{
    Type *type = ((Id *)lval)->getType();
    SymbolEntry *se = lval->getSymPtr();
    bool flag = true;
    if (type->isInt())
    {
        if (((IntType *)type)->isConst())
        {
            fprintf(stderr, "cannot assign to variable \'%s\' with const-qualified type \'%s\'\n",
                    ((IdentifierSymbolEntry *)se)->toStr().c_str(), type->toStr().c_str());
            flag = false;
        }
    }
    // if (flag && !expr->getType()->isInt())
    // {
    //     fprintf(stderr, "cannot initialize a variable of type \'int\' with an rvalue of type \'%s\'\n",
    //             expr->getType()->toStr().c_str());
    // }
}

// -----------只因Code代码区------------------

void Node::backPatch(std::vector<Instruction *> &list, BasicBlock *bb)
{
    for (auto &inst : list)
    {
        if (inst->isCond())
            dynamic_cast<CondBrInstruction *>(inst)->setTrueBranch(bb);
        else if (inst->isUncond())
            dynamic_cast<UncondBrInstruction *>(inst)->setBranch(bb);
    }
}

std::vector<Instruction *> Node::merge(std::vector<Instruction *> &list1, std::vector<Instruction *> &list2)
{
    std::vector<Instruction *> res(list1);
    res.insert(res.end(), list2.begin(), list2.end());
    return res;
}

void Ast::genCode(Unit *unit)
{
    IRBuilder *builder = new IRBuilder(unit);
    Node::setIRBuilder(builder);
    root->genCode();
}

void BinaryExpr::genCode()
{
    if (op == AND)
    {
        if (this->isConde())
        {
            expr1->setIsCond(true);
            expr2->setIsCond(true);
        }
        BasicBlock *expr2BB = new BasicBlock(now_func); // if the result of lhs is true, jump to the trueBB.
        expr1->genCode();
        backPatch(expr1->trueList(), expr2BB);
        builder->setInsertBB(expr2BB); // set the insert point to the trueBB so that intructions generated by expr2 will be inserted into it.
        expr2->genCode();
        true_list = expr2->trueList();
        false_list = merge(expr1->falseList(), expr2->falseList());
    }
    else if (op == OR)
    {
        // Todo
        if (this->isConde())
        {
            expr1->setIsCond(true);
            expr2->setIsCond(true);
        }
        BasicBlock *expr2BB = new BasicBlock(now_func);
        expr1->genCode();
        backPatch(expr1->falseList(), expr2BB);
        builder->setInsertBB(expr2BB);
        expr2->genCode();
        true_list = merge(expr1->trueList(), expr2->trueList());
        false_list = expr2->falseList();
    }
    else if (op >= LESS && op <= NOTEQUAL)
    {
        // Todo
        expr1->genCode();
        expr2->genCode();
        int cmpOps[] = {CmpInstruction::L, CmpInstruction::LE, CmpInstruction::G, CmpInstruction::GE, CmpInstruction::E, CmpInstruction::NE};
        new CmpInstruction(cmpOps[op - LESS], dst, expr1->getOperand(), expr2->getOperand(), now_bb);
        /* true和false未知，interB已知
        cmp
        br true, interB

        interB:
        b false
        */
        if (this->isConde())
        {
            BasicBlock *interB;
            interB = new BasicBlock(now_func);
            true_list.push_back(new CondBrInstruction(nullptr, interB, dst, now_bb));
            false_list.push_back(new UncondBrInstruction(nullptr, interB));
        }
    }
    else if (op >= ADD && op <= MOD)
    {
        expr1->genCode();
        expr2->genCode();
        int opcodes[] = {BinaryInstruction::ADD, BinaryInstruction::SUB, BinaryInstruction::MUL, BinaryInstruction::DIV, BinaryInstruction::MOD};
        new BinaryInstruction(opcodes[op - ADD], dst, expr1->getOperand(), expr2->getOperand(), now_bb);
    }
}

void UnaryExpr::genCode()
{
    expr->genCode();
    if (op == SUB)
    {
        new BinaryInstruction(BinaryInstruction::SUB, dst, new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), expr->getOperand(), now_bb);
    }
    else if (op == NOT)
    {
        new XorInstruction(dst, expr->getOperand(), now_bb);
        if (isCond)
        {
            BasicBlock *interB;
            interB = new BasicBlock(now_func);
            true_list.push_back(new CondBrInstruction(nullptr, interB, dst, now_bb));
            false_list.push_back(new UncondBrInstruction(nullptr, interB));
        }
    }
}

void CallExpr::genCode()
{
    std::vector<Operand *> rParams;
    ExprNode *tmp = param;
    while (tmp != nullptr)
    {
        tmp->genCode();
        rParams.push_back(tmp->getOperand());
        tmp = (ExprNode *)tmp->getNext();
    }
    new CallInstruction(dst, symbolEntry, rParams, now_bb);
}

void Constant::genCode()
{
    // we don't need to generate code.
}

void Id::genCode()
{
    IdentifierSymbolEntry *se = (IdentifierSymbolEntry *)symbolEntry;
    Operand *addr = se->getAddr();
    if (se->getType()->isArray())
    {
        // 先算地址
        ExprNode *tmp = index;
        std::vector<Operand *> offs;
        if (tmp == nullptr)
        {
            // 如果数组标识符没有索引，他应该是作为参数传递的，取数组指针就行
            // 生成一条gep指令返回就行
            offs.push_back(new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)));
            new GepInstruction(dst, addr, offs, now_bb);
            return;
        }
        while (tmp)
        {
            tmp->genCode();
            offs.push_back(tmp->getOperand());
            tmp = (ExprNode *)tmp->getNext();
        }
        addr = new Operand(new TemporarySymbolEntry(new PointerType(TypeSystem::intType), SymbolTable::getLabel()));
        new GepInstruction(addr, se->getAddr(), offs, now_bb);
    }
    else if (se->getType()->isPtr())
    {
        ExprNode *tmp = index;
        if (tmp == nullptr)
        {
            // 如果数组标识符没有索引，他应该是作为参数传递的，取数组指针就行
            // 生成一条gep指令返回就行
            new LoadInstruction(dst, addr, now_bb);
            return;
        }
        Operand *base = new Operand(new TemporarySymbolEntry(((PointerType *)(addr->getType()))->getType(), SymbolTable::getLabel()));
        new LoadInstruction(base, addr, now_bb);
        std::vector<Operand *> offs;
        while (tmp)
        {
            tmp->genCode();
            offs.push_back(tmp->getOperand());
            tmp = (ExprNode *)tmp->getNext();
        }
        addr = new Operand(new TemporarySymbolEntry(new PointerType(TypeSystem::intType), SymbolTable::getLabel()));
        new GepInstruction(addr, base, offs, now_bb, true);
    }
    new LoadInstruction(dst, addr, now_bb);
}

void ImplictCastExpr::genCode()
{
    expr->genCode();
    // bool转int
    if (b2i)
    {
        new ZextInstruction(dst, expr->getOperand(), true, now_bb);
    }
    // int转bool
    else
    {
        new CmpInstruction(CmpInstruction::NE, dst, expr->getOperand(), new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), now_bb);
        if (this->isCond)
        {
            BasicBlock *interB;
            interB = new BasicBlock(now_func);
            true_list.push_back(new CondBrInstruction(nullptr, interB, dst, now_bb));
            false_list.push_back(new UncondBrInstruction(nullptr, interB));
        }
    }
}

void CompoundStmt::genCode()
{
    // Todo
    if (stmt)
    {
        stmt->genCode();
    }
}

void SeqNode::genCode()
{
    // Todo
    stmt1->genCode();
    stmt2->genCode();
}

void DeclStmt::genCode()
{
    IdentifierSymbolEntry *se = dynamic_cast<IdentifierSymbolEntry *>(id->getSymPtr());
    if (se->isGlobal())
    {
        Operand *addr;
        SymbolEntry *addr_se;
        addr_se = new IdentifierSymbolEntry(*se);
        addr_se->setType(new PointerType(se->getType()));
        addr = new Operand(addr_se);
        se->setAddr(addr);
        unit.addGlobalVar(se);
    }
    else
    {
        BasicBlock *entry = now_func->getEntry();
        Instruction *alloca;
        Operand *addr;
        addr = new Operand(new TemporarySymbolEntry(new PointerType(se->getType()), SymbolTable::getLabel()));
        alloca = new AllocaInstruction(addr, se); // allocate space for local id in function stack.
        entry->insertFront(alloca);               // allocate instructions should be inserted into the begin of the entry block.
        se->setAddr(addr);                        // set the addr operand in symbol entry so that we can use it in subsequent code generation.
        if (expr)
        {
            expr->genCode();
            new StoreInstruction(addr, expr->getOperand(), now_bb);
        }
        if (se->isParam())
        {
            now_func->addParam(se->getArgAddr());
            new StoreInstruction(addr, se->getArgAddr(), now_bb);
        }
    }
    // 这里要看看下一个有没有
    if (this->getNext())
    {
        this->getNext()->genCode();
    }
}

void BlankStmt::genCode()
{
}

void IfStmt::genCode()
{
    BasicBlock *then_bb, *end_bb;

    then_bb = new BasicBlock(now_func);
    end_bb = new BasicBlock(now_func);

    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    builder->setInsertBB(end_bb);
}

void IfElseStmt::genCode()
{
    // Todo
    BasicBlock *then_bb, *else_bb, *end_bb;

    then_bb = new BasicBlock(now_func);
    else_bb = new BasicBlock(now_func);
    end_bb = new BasicBlock(now_func);

    // 生成cond
    cond->genCode();
    backPatch(cond->trueList(), then_bb);
    backPatch(cond->falseList(), else_bb);

    // 生成then
    builder->setInsertBB(then_bb);
    thenStmt->genCode();
    then_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, then_bb);

    // 生成else
    builder->setInsertBB(else_bb);
    elseStmt->genCode();
    else_bb = builder->getInsertBB();
    new UncondBrInstruction(end_bb, else_bb);

    builder->setInsertBB(end_bb);
}

void WhileStmt::genCode()
{
    BasicBlock *cond_bb, *stmt_bb, *end_bb;
    cond_bb = new BasicBlock(now_func);
    stmt_bb = new BasicBlock(now_func);
    end_bb = new BasicBlock(now_func);
    this->cond_bb = cond_bb;
    this->end_bb = end_bb;

    new UncondBrInstruction(cond_bb, now_bb);
    builder->setInsertBB(cond_bb);
    cond->genCode();
    backPatch(cond->trueList(), stmt_bb);
    backPatch(cond->falseList(), end_bb);

    builder->setInsertBB(stmt_bb);
    stmt->genCode();
    stmt_bb = builder->getInsertBB();
    new UncondBrInstruction(cond_bb, stmt_bb);

    builder->setInsertBB(end_bb);
}

void BreakStmt::genCode()
{
    if (whileStmt)
    {
        new UncondBrInstruction(((WhileStmt *)whileStmt)->get_end_bb(), now_bb);
        builder->setInsertBB(new BasicBlock(now_func));
    }
}

void ContinueStmt::genCode()
{
    if (whileStmt)
    {
        new UncondBrInstruction(((WhileStmt *)whileStmt)->get_cond_bb(), now_bb);
        builder->setInsertBB(new BasicBlock(now_func));
    }
}

void ReturnStmt::genCode()
{
    // Todo
    if (retValue)
    {
        retValue->genCode();
    }
    new RetInstruction(retValue ? retValue->getOperand() : nullptr, now_bb);
}

void AssignStmt::genCode()
{
    IdentifierSymbolEntry *se = (IdentifierSymbolEntry *)lval->getSymPtr();
    expr->genCode();
    Operand *addr = se->getAddr();
    Operand *src = expr->getOperand();
    /***
     * We haven't implemented array yet, the lval can only be ID. So we just store the result of the `expr` to the addr of the id.
     * If you want to implement array, you have to caculate the address first and then store the result into it.
     */
    if (se->getType()->isArray())
    {
        // 先算地址
        ExprNode *index = ((Id *)lval)->getIndex();
        std::vector<Operand *> offs;
        while (index)
        {
            index->genCode();
            offs.push_back(index->getOperand());
            index = (ExprNode *)index->getNext();
        }
        addr = new Operand(new TemporarySymbolEntry(new PointerType(TypeSystem::intType), SymbolTable::getLabel()));
        new GepInstruction(addr, se->getAddr(), offs, now_bb);
    }
    else if (se->getType()->isPtr())
    {
        Operand *base = new Operand(new TemporarySymbolEntry(((PointerType *)(addr->getType()))->getType(), SymbolTable::getLabel()));
        new LoadInstruction(base, addr, now_bb);
        ExprNode *tmp = ((Id *)lval)->getIndex();
        std::vector<Operand *> offs;
        while (tmp)
        {
            tmp->genCode();
            offs.push_back(tmp->getOperand());
            tmp = (ExprNode *)tmp->getNext();
        }
        addr = new Operand(new TemporarySymbolEntry(new PointerType(TypeSystem::intType), SymbolTable::getLabel()));
        new GepInstruction(addr, base, offs, now_bb, true);
    }
    new StoreInstruction(addr, src, now_bb);
}

void ExprStmt::genCode()
{
    expr->genCode();
}

void FunctionDef::genCode()
{
    Unit *unit = builder->getUnit();
    Function *func = new Function(unit, se);
    BasicBlock *entry = func->getEntry();
    // set the insert point to the entry basicblock of this function.
    builder->setInsertBB(entry);

    if (decl != nullptr)
    {
        decl->genCode();
    }
    stmt->genCode();

    /**
     * Construct control flow graph. You need do set successors and predecessors for each basic block.
     * Todo
     */
    for (auto b = func->begin(); b != func->end(); b++)
    {
        Instruction *lastIns = (*b)->rbegin();
        if (lastIns->isCond())
        {
            BasicBlock *trueBranch = ((CondBrInstruction *)lastIns)->getTrueBranch();
            BasicBlock *falseBranch = ((CondBrInstruction *)lastIns)->getFalseBranch();
            (*b)->addSucc(trueBranch);
            (*b)->addSucc(falseBranch);
            trueBranch->addPred(*b);
            falseBranch->addPred(*b);
        }
        else if (lastIns->isUncond())
        {
            BasicBlock *branch = ((UncondBrInstruction *)lastIns)->getBranch();
            (*b)->addSucc(branch);
            branch->addPred(*b);
        }
        else if (!lastIns->isRet())
        { // 如果这个块没有后继，而且没有ret
            if (((FunctionType *)now_func->getSymPtr()->getType())->getRetType() == TypeSystem::intType)
            {
                new RetInstruction(new Operand(new ConstantSymbolEntry(TypeSystem::intType, 0)), (*b));
            }
            else if (((FunctionType *)now_func->getSymPtr()->getType())->getRetType() == TypeSystem::voidType)
            {
                new RetInstruction(nullptr, (*b));
            }
        }
    }
}

// typeCheck代码区

void Ast::typeCheck()
{
    if (root != nullptr)
        return root->typeCheck();
}

void BinaryExpr::typeCheck()
{
}

void UnaryExpr::typeCheck()
{
}

void CallExpr::typeCheck()
{
}

void Constant::typeCheck()
{
}

void Id::typeCheck()
{
}

void CompoundStmt::typeCheck()
{
}

void SeqNode::typeCheck()
{
}

void DeclStmt::typeCheck()
{
}

void IfStmt::typeCheck()
{
}

void IfElseStmt::typeCheck()
{
}

void WhileStmt::typeCheck()
{
}

void ReturnStmt::typeCheck()
{
}

void AssignStmt::typeCheck()
{
}

void ExprStmt::typeCheck()
{
}

void FunctionDef::typeCheck()
{
}

// output代码区

void Ast::output()
{
    fprintf(yyout, "program\n");
    if (root != nullptr)
        root->output(4);
}

void ExprNode::output(int level)
{
    std::string name, type;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    fprintf(yyout, "%*cconst string\ttype:%s\t%s\n", level, ' ', type.c_str(), name.c_str());
}

void BinaryExpr::output(int level)
{
    std::string op_str;
    switch (op)
    {
    case ADD:
        op_str = "add";
        break;
    case SUB:
        op_str = "sub";
        break;
    case MUL:
        op_str = "mul";
        break;
    case DIV:
        op_str = "div";
        break;
    case MOD:
        op_str = "mod";
        break;
    case AND:
        op_str = "and";
        break;
    case OR:
        op_str = "or";
        break;
    case LESS:
        op_str = "less";
        break;
    case LESSEQUAL:
        op_str = "lessequal";
        break;
    case GREATER:
        op_str = "greater";
        break;
    case GREATEREQUAL:
        op_str = "greaterequal";
        break;
    case EQUAL:
        op_str = "equal";
        break;
    case NOTEQUAL:
        op_str = "notequal";
        break;
    }
    fprintf(yyout, "%*cBinaryExpr\top: %s\ttype: %s\n", level, ' ',
            op_str.c_str(), type->toStr().c_str());
    expr1->output(level + 4);
    expr2->output(level + 4);
}

void UnaryExpr::output(int level)
{
    std::string op_str;
    switch (op)
    {
    case NOT:
        op_str = "not";
        break;
    case SUB:
        op_str = "minus";
        break;
    }
    fprintf(yyout, "%*cUnaryExpr\top: %s\ttype: %s\n", level, ' ',
            op_str.c_str(), type->toStr().c_str());
    expr->output(level + 4);
}

void CallExpr::output(int level)
{
    std::string name, type;
    int scope;
    if (symbolEntry)
    {
        name = symbolEntry->toStr();
        type = symbolEntry->getType()->toStr();
        scope = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getScope();
        fprintf(yyout, "%*cCallExpr\tfunction name: %s\tscope: %d\ttype: %s\n", level, ' ', name.c_str(), scope, type.c_str());
        Node *temp = param;
        // 打印参数信息
        while (temp)
        {
            temp->output(level + 4);
            temp = temp->getNext();
        }
    }
}

void Constant::output(int level)
{
    std::string type, value;
    type = symbolEntry->getType()->toStr();
    value = symbolEntry->toStr();
    fprintf(yyout, "%*cIntegerLiteral\tvalue: %s\ttype: %s\n", level, ' ',
            value.c_str(), type.c_str());
}

void Id::output(int level)
{
    std::string name, type;
    int scope;
    name = symbolEntry->toStr();
    type = symbolEntry->getType()->toStr();
    scope = dynamic_cast<IdentifierSymbolEntry *>(symbolEntry)->getScope();
    fprintf(yyout, "%*cId\tname: %s\tscope: %d\ttype: %s\n", level, ' ', name.c_str(), scope, type.c_str());
}

void ImplictCastExpr::output(int level)
{
    fprintf(yyout, "%*cImplictCastExpr\ttype: %s to %s\n", level, ' ', expr->getType()->toStr().c_str(), type->toStr().c_str());
    this->expr->output(level + 4);
}

void CompoundStmt::output(int level)
{
    fprintf(yyout, "%*cCompoundStmt\n", level, ' ');
    stmt->output(level + 4);
}

void SeqNode::output(int level)
{
    stmt1->output(level);
    stmt2->output(level);
}

void DeclStmt::output(int level)
{
    fprintf(yyout, "%*cDeclStmt\n", level, ' ');
    id->output(level + 4);
    if (expr)
    {
        expr->output(level + 4);
    }
    if (this->getNext())
    {
        ((DeclStmt *)getNext())->output(level);
    }
}

void BlankStmt::output(int level)
{
    fprintf(yyout, "%*cBlankStmt\n", level, ' ');
}

void IfStmt::output(int level)
{
    fprintf(yyout, "%*cIfStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
}

void IfElseStmt::output(int level)
{
    fprintf(yyout, "%*cIfElseStmt\n", level, ' ');
    cond->output(level + 4);
    thenStmt->output(level + 4);
    elseStmt->output(level + 4);
}

void WhileStmt::output(int level)
{
    fprintf(yyout, "%*cWhileStmt\n", level, ' ');
    cond->output(level + 4);
    stmt->output(level + 4);
}
void BreakStmt::output(int level)
{
    fprintf(yyout, "%*cBreakStmt\n", level, ' ');
}

void ContinueStmt::output(int level)
{
    fprintf(yyout, "%*cContinueStmt\n", level, ' ');
}

void ReturnStmt::output(int level)
{
    fprintf(yyout, "%*cReturnStmt\n", level, ' ');
    if (retValue != nullptr)
    {
        retValue->output(level + 4);
    }
}

void AssignStmt::output(int level)
{
    fprintf(yyout, "%*cAssignStmt\n", level, ' ');
    lval->output(level + 4);
    expr->output(level + 4);
}

void ExprStmt::output(int level)
{
    fprintf(yyout, "%*cExprStmt\n", level, ' ');
    expr->output(level + 4);
}

void FunctionDef::output(int level)
{
    std::string name, type;
    name = se->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "%*cFunctionDefine function name: %s, type: %s\n", level, ' ',
            name.c_str(), type.c_str());
    if (decl)
    {
        decl->output(level + 4);
    }
    stmt->output(level + 4);
}
