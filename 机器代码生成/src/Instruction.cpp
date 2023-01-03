#include "Instruction.h"
#include "BasicBlock.h"
#include <iostream>
#include <cmath>
#include "Function.h"
#include "Type.h"
#include "MachineCode.h"
extern FILE *yyout;

Instruction::Instruction(unsigned instType, BasicBlock *insert_bb)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr)
    {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
}

Instruction::~Instruction()
{
    parent->remove(this);
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}

AllocaInstruction::AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    this->se = se;
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const
{
    std::string dst, type;
    dst = operands[0]->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src_addr);
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();
    src_type = operands[1]->getType()->toStr();
    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src_type.c_str(), src.c_str());
}

StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(STORE, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
}

StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();
    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();

    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(), src.c_str(), dst_type.c_str(), dst.c_str());
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void BinaryInstruction::output() const
{
    std::string dst, src1, src2, op, type;
    dst = operands[0]->toStr();
    src1 = operands[1]->toStr();
    src2 = operands[2]->toStr();
    type = operands[0]->getType()->toStr();
    switch (opcode)
    {
    case ADD:
        op = "add";
        break;
    case SUB:
        op = "sub";
        break;
    case MUL:
        op = "mul";
        break;
    case DIV:
        op = "sdiv";
        break;
    case MOD:
        op = "srem";
        break;
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", dst.c_str(), op.c_str(), type.c_str(), src1.c_str(), src2.c_str());
}

CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(CMP, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if (operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    std::string dst, src1, src2, op, type;
    dst = operands[0]->toStr();
    src1 = operands[1]->toStr();
    src2 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();
    switch (opcode)
    {
    case E:
        op = "eq";
        break;
    case NE:
        op = "ne";
        break;
    case L:
        op = "slt";
        break;
    case LE:
        op = "sle";
        break;
    case G:
        op = "sgt";
        break;
    case GE:
        op = "sge";
        break;
    default:
        op = "";
        break;
    }

    fprintf(yyout, "  %s = icmp %s %s %s, %s\n", dst.c_str(), op.c_str(), type.c_str(), src1.c_str(), src2.c_str());
}

UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    branch = to;
}

void UncondBrInstruction::output() const
{
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
}

void UncondBrInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock *true_branch, BasicBlock *false_branch, Operand *cond, BasicBlock *insert_bb) : Instruction(COND, insert_bb)
{
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const
{
    std::string cond, type;
    cond = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(), cond.c_str(), true_label, false_label);
}

void CondBrInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb)
{
    true_branch = bb;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

CallInstruction::CallInstruction(Operand *dst, SymbolEntry *func, std::vector<Operand *> params, BasicBlock *insert_bb) : Instruction(CALL, insert_bb)
{
    operands.push_back(dst);
    if (dst != nullptr)
    {
        dst->setDef(this);
    }
    for (auto operand : params)
    {
        operands.push_back(operand);
        operand->addUse(this);
    }
    this->func = func;
}

CallInstruction::~CallInstruction() {}

void CallInstruction::output() const
{
    fprintf(yyout, "  ");
    if (operands[0] != nullptr)
    {
        fprintf(yyout, "%s = ", operands[0]->toStr().c_str());
    }
    fprintf(yyout, "call %s %s(", ((FunctionType *)(func->getType()))->getRetType()->toStr().c_str(), func->toStr().c_str());
    for (long unsigned int i = 1; i < operands.size(); i++)
    {
        if (i != 1)
        {
            fprintf(yyout, ", ");
        }
        fprintf(yyout, "%s %s", operands[i]->getType()->toStr().c_str(), operands[i]->toStr().c_str());
    }
    fprintf(yyout, ")\n");
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    if (src != nullptr)
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if (!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const
{
    if (operands.empty())
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string ret, type;
        ret = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret.c_str());
    }
}

XorInstruction::XorInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb) : Instruction(XOR, insert_bb)
{
    dst->setDef(this);
    src->addUse(this);
    operands.push_back(dst);
    operands.push_back(src);
}

void XorInstruction::output() const
{
    std::string dst, src;
    dst = operands[0]->toStr();
    src = operands[1]->toStr();
    fprintf(yyout, "  %s = xor i1 %s, true\n", dst.c_str(), src.c_str());
}

ZextInstruction::ZextInstruction(Operand *dst, Operand *src, bool b2i, BasicBlock *insert_bb) : Instruction(ZEXT, insert_bb)
{
    this->b2i = b2i;
    dst->setDef(this);
    src->addUse(this);
    operands.push_back(dst);
    operands.push_back(src);
}

void ZextInstruction::output() const
{
    std::string dst, src;
    dst = operands[0]->toStr();
    src = operands[1]->toStr();
    if (b2i)
    {
        fprintf(yyout, "  %s = zext i1 %s to i32\n", dst.c_str(), src.c_str());
    }
    else
    {

        fprintf(yyout, "  %s = zext i32 %s to i1\n", dst.c_str(), src.c_str());
    }
}
MachineOperand *Instruction::genMachineOperand(Operand *ope, AsmBuilder *builder = nullptr)
{
    auto se = ope->getEntry();
    MachineOperand *mope = nullptr;
    if (se->isConstant())
        mope = new MachineOperand(MachineOperand::IMM, dynamic_cast<ConstantSymbolEntry *>(se)->getValue());
    else if (se->isTemporary())
    {
        if (((TemporarySymbolEntry *)se)->isParam() && builder)
        {
            int argNum = dynamic_cast<TemporarySymbolEntry *>(se)->getArgNum();
            if (argNum < 4)
            {
                mope = new MachineOperand(MachineOperand::REG, argNum);
            }
            else
            { // 要从栈里加载
                mope = new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
                auto cur_block = builder->getBlock();
                auto cur_inst = new LoadMInstruction(cur_block, new MachineOperand(*mope), new MachineOperand(MachineOperand::REG, 11), new MachineOperand(MachineOperand::IMM, 4 * (argNum - 4)));
                cur_block->InsertInst(cur_inst);
                cur_block->addUInst(cur_inst);
            }
        }
        else
        {
            mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry *>(se)->getLabel());
        }
    }
    else if (se->isVariable())
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry *>(se);
        if (id_se->isGlobal())
            mope = new MachineOperand(id_se->toStr().c_str() + 1);
        else
            exit(0);
    }
    return mope;
}

MachineOperand *Instruction::genMachineReg(int reg)
{
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand *Instruction::genMachineVReg()
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
}

MachineOperand *Instruction::genMachineImm(int val)
{
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand *Instruction::genMachineLabel(int block_no)
{
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder *builder)
{
    /* HINT:
     * Allocate stack space for local variabel
     * Store frame offset in symbol entry */
    auto cur_func = builder->getFunction();
    int offset = cur_func->AllocSpace(se->getType()->getSize() / TypeSystem::intType->getSize() * 4);
    dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())->setOffset(-offset);
}

void LoadInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst = nullptr;
    // Load global operand
    if (operands[1]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry *>(operands[1]->getEntry())->isGlobal())
    {
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto src = genMachineOperand(operands[1]);
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
        cur_block->InsertInst(cur_inst);
        // example: load r1, [r0]
        cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // Load local operand
    else if (operands[1]->getEntry()->isTemporary() && operands[1]->getDef() && operands[1]->getDef()->isAlloc())
    {
        // example: load r1, [r0, #4]
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineReg(11);
        int offset = dynamic_cast<TemporarySymbolEntry *>(operands[1]->getEntry())->getOffset();
        auto src2 = genMachineImm(offset);
        if (abs(offset) <= 16384)
        {
            cur_inst = new LoadMInstruction(cur_block, dst, src1, src2);
            cur_block->InsertInst(cur_inst);
        }
        else
        {
            auto internal_reg = genMachineVReg();
            cur_block->InsertInst(new LoadMInstruction(cur_block, internal_reg, src2));
            cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, new MachineOperand(*internal_reg), new MachineOperand(*internal_reg), src1));
            cur_block->InsertInst(new LoadMInstruction(cur_block, dst, new MachineOperand(*internal_reg)));
        }
    }
    // Load operand from temporary variable
    else
    {
        // example: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void StoreInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO
    auto cur_block = builder->getBlock();
    MachineInstruction *cur_inst = nullptr;
    auto src = genMachineOperand(operands[1], builder);
    if (src->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, internal_reg, src);
        cur_block->InsertInst(cur_inst);
        src = new MachineOperand(*internal_reg);
    }
    // Store global operand
    if (operands[0]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry *>(operands[0]->getEntry())->isGlobal())
    {
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, dst);
        cur_block->InsertInst(cur_inst);
        // example: store r1, [r0]
        cur_inst = new StoreMInstruction(cur_block, src, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // Load local operand
    else if (operands[0]->getEntry()->isTemporary() && operands[0]->getDef() && operands[0]->getDef()->isAlloc())
    {
        // example: store r1, [r0, #4]
        auto dst = genMachineReg(11);
        // auto off = genMachineImm(dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())->getOffset());
        // cur_inst = new StoreMInstruction(cur_block, src, dst, off);
        // cur_block->InsertInst(cur_inst);
        int offset = dynamic_cast<TemporarySymbolEntry *>(operands[0]->getEntry())->getOffset();
        auto off = genMachineImm(offset);
        if (abs(offset) <= 16384)
        {
            cur_inst = new StoreMInstruction(cur_block, src, dst, off);
            cur_block->InsertInst(cur_inst);
        }
        else
        {
            auto internal_reg = genMachineVReg();
            cur_block->InsertInst(new LoadMInstruction(cur_block, internal_reg, off));
            cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, new MachineOperand(*internal_reg), new MachineOperand(*internal_reg), dst));
            cur_block->InsertInst(new StoreMInstruction(cur_block, src, new MachineOperand(*internal_reg)));
        }
    }
    // Load operand from temporary variable
    else
    {
        // example: store r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        cur_inst = new StoreMInstruction(cur_block, src, dst);
        cur_block->InsertInst(cur_inst);
    }
}

void BinaryInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO:
    // complete other instructions
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    /* HINT:
     * The source operands of ADD instruction in ir code both can be immediate num.
     * However, it's not allowed in assembly code.
     * So you need to insert LOAD/MOV instrucrion to load immediate num into register.
     * As to other instructions, such as MUL, CMP, you need to deal with this situation, too.*/
    MachineInstruction *cur_inst = nullptr;
    if (src1->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_inst = new LoadMInstruction(cur_block, internal_reg, src1);
        cur_block->InsertInst(cur_inst);
        src1 = new MachineOperand(*internal_reg);
    }
    if (src2->isImm() && (abs(src2->getVal()) > 255 || opcode == MUL || opcode == DIV || opcode == MOD))
    {
        // 按理说src1也要这么写，但是懒得改了
        int value = src2->getVal();
        bool isNeg = false;
        if (value < 0)
        {
            isNeg = true;
            value = -value;
        }
        int ele = (1 << 30);
        auto internal_reg = genMachineVReg();
        cur_block->InsertInst(new LoadMInstruction(cur_block, internal_reg, genMachineImm(0)));
        while (value > 255)
        {
            if (value >= ele)
            {
                value -= ele;
                cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, new MachineOperand(*internal_reg), new MachineOperand(*internal_reg), genMachineImm(ele)));
            }
            ele /= 2;
        }
        if (value > 0)
        {
            cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, new MachineOperand(*internal_reg), new MachineOperand(*internal_reg), genMachineImm(value)));
        }
        if (isNeg)
        {
            auto internal_reg2 = genMachineVReg();
            cur_block->InsertInst(new LoadMInstruction(cur_block, internal_reg2, genMachineImm(0)));
            cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, new MachineOperand(*internal_reg), new MachineOperand(*internal_reg2), new MachineOperand(*internal_reg)));
        }
        src2 = new MachineOperand(*internal_reg);
    }
    switch (opcode)
    {
    case ADD:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
        break;
    case SUB:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);
        break;
    case MUL:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst, src1, src2);
        break;
    case DIV:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
        break;
    case AND:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::AND, dst, src1, src2);
        break;
    case OR:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::OR, dst, src1, src2);
        break;
    case MOD:
        // arm里没有模指令，要除乘减结合，来算余数
        {
            cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2));
            cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, new MachineOperand(*dst), new MachineOperand(*dst), new MachineOperand(*src2)));
            cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, new MachineOperand(*dst), new MachineOperand(*src1), new MachineOperand(*dst));
        }
        break;
    default:
        break;
    }
    cur_block->InsertInst(cur_inst);
}

void CmpInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO
    // 简简单单生成一条cmp指令就行
    auto cur_block = builder->getBlock();
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    if (src1->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_block->InsertInst(new LoadMInstruction(cur_block, internal_reg, src1));
        src1 = new MachineOperand(*internal_reg);
    }
    if (src2->isImm())
    {
        auto internal_reg = genMachineVReg();
        cur_block->InsertInst(new LoadMInstruction(cur_block, internal_reg, src2));
        src2 = new MachineOperand(*internal_reg);
    }
    cur_block->InsertInst(new CmpMInstruction(cur_block, src1, src2));
    // 这里借助builder向br指令传cond
    int cmpOpCode = 0, minusOpCode = 0;
    switch (opcode)
    {
    case E:
    {
        cmpOpCode = CmpMInstruction::EQ;
        minusOpCode = CmpMInstruction::NE;
    }
    break;
    case NE:
    {
        cmpOpCode = CmpMInstruction::NE;
        minusOpCode = CmpMInstruction::EQ;
    }
    break;
    case L:
    {
        cmpOpCode = CmpMInstruction::LT;
        minusOpCode = CmpMInstruction::GE;
    }
    break;
    case LE:
    {
        cmpOpCode = CmpMInstruction::LE;
        minusOpCode = CmpMInstruction::GT;
    }
    break;
    case G:
    {
        cmpOpCode = CmpMInstruction::GT;
        minusOpCode = CmpMInstruction::LE;
    }
    break;
    case GE:
    {
        cmpOpCode = CmpMInstruction::GE;
        minusOpCode = CmpMInstruction::LT;
    }
    break;
    default:
        break;
    }
    auto dst = genMachineOperand(operands[0]);
    cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, dst, genMachineImm(1), cmpOpCode));
    cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, new MachineOperand(*dst), genMachineImm(0), minusOpCode));
    builder->setCmpOpcode(cmpOpCode);
}

void UncondBrInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO
    // 直接生成一条指令就行
    auto cur_block = builder->getBlock();
    cur_block->InsertInst(new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(branch->getNo())));
}

void CondBrInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO
    // 生成两条指令
    auto cur_block = builder->getBlock();
    // auto src = genMachineOperand(operands[0]);
    // cur_block->InsertInst(new CmpMInstruction(cur_block, src, genMachineImm(0)));
    // cur_block->InsertInst(new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(true_branch->getNo()), CmpMInstruction::NE));
    cur_block->InsertInst(new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(true_branch->getNo()), builder->getCmpOpcode()));
    cur_block->InsertInst(new BranchMInstruction(cur_block, BranchMInstruction::B, genMachineLabel(false_branch->getNo())));
}

void CallInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    for (long unsigned int i = 1; i <= operands.size() - 1 && i <= 4; i++)
    {
        // 前四个参数用r0-r3传
        auto param = genMachineOperand(operands[i]);
        // 用mov指令把参数放到对应寄存器里
        cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, genMachineReg(i - 1), param));
    }
    int param_size_in_stack = 0;
    for (long unsigned int i = operands.size() - 1; i >= 5; i--)
    {
        // 参数个数大于4，要push了，从右向左push
        auto param = genMachineOperand(operands[i]);
        if (param->isImm())
        {
            auto internal_reg = genMachineVReg();
            cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, internal_reg, param));
            param = new MachineOperand(*internal_reg);
        }
        cur_block->InsertInst(new StackMInstrcuton(cur_block, StackMInstrcuton::PUSH, {param}));
        param_size_in_stack += 4;
    }
    // 生成bl指令，调用函数
    cur_block->InsertInst(new BranchMInstruction(cur_block, BranchMInstruction::BL, new MachineOperand(func->toStr().c_str())));
    // 生成add指令释放栈空间
    if (param_size_in_stack > 0)
    {
        auto sp = genMachineReg(13);
        cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, sp, sp, genMachineImm(param_size_in_stack)));
    }
    if (operands[0])
    {
        cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, genMachineOperand(operands[0]), genMachineReg(0)));
    }
}

void RetInstruction::genMachineCode(AsmBuilder *builder)
{
    // TODO
    /* HINT:
     * 1. Generate mov instruction to save return value in r0
     * 2. Restore callee saved registers and sp, fp
     * 3. Generate bx instruction */
    auto cur_bb = builder->getBlock();
    // 如果有返回值
    if (operands.size() > 0)
    {
        cur_bb->InsertInst(new MovMInstruction(cur_bb, MovMInstruction::MOV, genMachineReg(0), genMachineOperand(operands[0])));
    }
    auto sp = genMachineReg(13);
    // 释放栈空间，这里也不知道，因为寄存器分配之后可能会溢出到栈里面，这里也先意思意思
    int stack_size = builder->getFunction()->AllocSpace(0);
    auto cur_inst = new BinaryMInstruction(cur_bb, BinaryMInstruction::ADD, sp, sp, genMachineImm(stack_size));
    cur_bb->InsertInst(cur_inst);
    cur_bb->addUInst(cur_inst);
    // 恢复保存的寄存器，这里还不知道，先欠着
    auto curr_inst = new StackMInstrcuton(cur_bb, StackMInstrcuton::POP, {});
    cur_bb->InsertInst(curr_inst);
    cur_bb->addUInst(curr_inst);
    // bx指令
    cur_bb->InsertInst(new BranchMInstruction(cur_bb, BranchMInstruction::BX, genMachineReg(14)));
}

void XorInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    cur_block->InsertInst(new CmpMInstruction(cur_block, src, genMachineImm(0)));
    cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, dst, genMachineImm(1), CmpMInstruction::EQ));
    cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, new MachineOperand(*dst), genMachineImm(0), CmpMInstruction::NE));
    builder->setCmpOpcode(CmpMInstruction::EQ);
}

void ZextInstruction::genMachineCode(AsmBuilder *builder)
{
    // 生成一条mov指令得了
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto src = genMachineOperand(operands[1]);
    cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, dst, src));
}

void GepInstruction::genMachineCode(AsmBuilder *builder)
{
    // type2表示是不是通过传参传过来的数组指针，为true表示是，否则表示局部变量或者全局变量
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);
    auto base = type2 ? genMachineOperand(operands[1]) : genMachineVReg();
    // 全局变量，先load
    if (operands[1]->getEntry()->isVariable() && dynamic_cast<IdentifierSymbolEntry *>(operands[1]->getEntry())->isGlobal())
    {
        auto src = genMachineOperand(operands[1]);
        cur_block->InsertInst(new LoadMInstruction(cur_block, base, src));
        base = new MachineOperand(*base);
    }
    else if (!type2) // 局部变量
    {
        // 偏移都是负数
        int offset = ((TemporarySymbolEntry *)operands[1]->getEntry())->getOffset();
        if (offset >= -254)
        {
            cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, base, genMachineReg(11), genMachineImm(offset)));
            base = new MachineOperand(*base);
        }
        else
        {
            // 拆成2的幂次，先加后减
            auto internal_reg = genMachineVReg();
            cur_block->InsertInst(new LoadMInstruction(cur_block, internal_reg, genMachineImm(0)));
            offset = -offset;
            int ele = 16384;
            while (offset > 255)
            {
                if (offset >= ele)
                {
                    offset -= ele;
                    cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, new MachineOperand(*internal_reg), new MachineOperand(*internal_reg), genMachineImm(ele)));
                }
                ele /= 2;
            }
            if (offset > 0)
            {
                cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, new MachineOperand(*internal_reg), new MachineOperand(*internal_reg), genMachineImm(offset)));
            }
            cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, base, genMachineReg(11), new MachineOperand(*internal_reg)));
            base = new MachineOperand(*base);
        }
        // else
        // {
        //     auto internal_reg = genMachineVReg();
        //     cur_block->InsertInst(new LoadMInstruction(cur_block, internal_reg, genMachineImm(offset)));
        //     cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, base, genMachineReg(11), new MachineOperand(*internal_reg)));
        //     base = new MachineOperand(*base);
        // }
    }
    Type *arrType = ((PointerType *)operands[1]->getType())->getType();
    std::vector<int> indexs = ((ArrayType *)arrType)->getIndexs();
    std::vector<int> imms; // 这个专门用来记录索引中的立即数比如说a[10][i][3] 就存一个{0, 2}这样子
    for (unsigned long int i = 2; i < operands.size(); i++)
    {
        if (operands[i]->getEntry()->isConstant())
        {
            // 为了省代码，所有的立即数一起算，这里先跳过
            imms.push_back(i);
            continue;
        }
        unsigned int step = 4;
        for (unsigned long int j = i - (type2 ? 2 : 1); j < indexs.size(); j++)
        {
            step *= indexs[j];
        }
        auto off = genMachineVReg();
        cur_block->InsertInst(new LoadMInstruction(cur_block, off, genMachineImm(step)));
        auto internal_reg1 = genMachineVReg();
        auto src1 = genMachineOperand(operands[i]);
        cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, internal_reg1, src1, off));
        auto internal_reg2 = genMachineVReg();
        cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, internal_reg2, new MachineOperand(*base), new MachineOperand(*internal_reg1)));
        base = new MachineOperand(*internal_reg2);
    }
    int off = 0;
    for (auto index : imms)
    {
        int imm = ((ConstantSymbolEntry *)operands[index]->getEntry())->getValue();
        unsigned int step = 4;
        for (unsigned long int j = index - (type2 ? 2 : 1); j < indexs.size(); j++)
        {
            step *= indexs[j];
        }
        off += (imm * step);
    }
    if (off > 0)
    {
        auto internal_reg1 = genMachineImm(off);
        if (abs(off) > 16384)
        {
            auto internal_reg2 = genMachineVReg();
            cur_block->InsertInst(new LoadMInstruction(cur_block, internal_reg2, internal_reg1));
            internal_reg1 = new MachineOperand(*internal_reg2);
        }
        cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, new MachineOperand(*base), new MachineOperand(*base), new MachineOperand(*internal_reg1)));
    }
    // for (unsigned long int i = 2; i < operands.size(); i++)
    // {
    //     unsigned int step = 4;
    //     for (unsigned long int j = i - (type2 ? 2 : 1); j < indexs.size(); j++)
    //     {
    //         step *= indexs[j];
    //     }
    //     auto off = genMachineVReg();
    //     cur_block->InsertInst(new LoadMInstruction(cur_block, off, genMachineImm(step)));
    //     auto internal_reg1 = genMachineVReg();
    //     auto src1 = genMachineOperand(operands[i]);
    //     if (src1->isImm())
    //     {
    //         auto internal_reg = genMachineVReg();
    //         cur_block->InsertInst(new LoadMInstruction(cur_block, internal_reg, src1));
    //         src1 = new MachineOperand(*internal_reg);
    //     }
    //     cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, internal_reg1, src1, off));
    //     auto internal_reg2 = genMachineVReg();
    //     cur_block->InsertInst(new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, internal_reg2, new MachineOperand(*base), new MachineOperand(*internal_reg1)));
    //     base = new MachineOperand(*internal_reg2);
    // }
    cur_block->InsertInst(new MovMInstruction(cur_block, MovMInstruction::MOV, dst, base));
}

GepInstruction::GepInstruction(Operand *dst, Operand *base, std::vector<Operand *> offs, BasicBlock *insert_bb, bool type2) : Instruction(GEP, insert_bb), type2(type2)
{
    operands.push_back(dst);
    operands.push_back(base);
    dst->setDef(this);
    base->addUse(this);
    for (auto off : offs)
    {
        operands.push_back(off);
        off->addUse(this);
    }
}

void GepInstruction::output() const
{
    Operand *dst = operands[0];
    Operand *base = operands[1];
    std::string arrType = base->getType()->toStr();
    if (!type2)
    {
        fprintf(yyout, "  %s = getelementptr inbounds %s, %s %s, i32 0",
                dst->toStr().c_str(), arrType.substr(0, arrType.size() - 1).c_str(),
                arrType.c_str(), base->toStr().c_str());
    }
    else
    {
        fprintf(yyout, "  %s = getelementptr inbounds %s, %s %s",
                dst->toStr().c_str(), arrType.substr(0, arrType.size() - 1).c_str(),
                arrType.c_str(), base->toStr().c_str());
    }
    for (unsigned long int i = 2; i < operands.size(); i++)
    {
        fprintf(yyout, ", i32 %s", operands[i]->toStr().c_str());
    }
    fprintf(yyout, "\n");
}
