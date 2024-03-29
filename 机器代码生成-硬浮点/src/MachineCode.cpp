#include "MachineCode.h"
#include "AsmBuilder.h"
#include "Type.h"
#include <iostream>
extern FILE *yyout;

MachineOperand::MachineOperand(int tp, int val, bool fpu)
{
    this->type = tp;
    this->fpu = fpu;
    if (tp == MachineOperand::IMM)
        this->val = val;
    else
        this->reg_no = val;
}

MachineOperand::MachineOperand(std::string label)
{
    this->type = MachineOperand::LABEL;
    this->label = label;
}

bool MachineOperand::operator==(const MachineOperand &a) const
{
    if (this->type != a.type)
        return false;
    if (this->fpu != a.fpu)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

bool MachineOperand::operator<(const MachineOperand &a) const
{
    if (this->type == a.type)
    {
        if (this->type == IMM)
            return this->val < a.val;
        return this->reg_no < a.reg_no;
    }
    return this->type < a.type;

    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

void MachineOperand::PrintReg()
{
    if (fpu)
    { // 浮点寄存器用s
        fprintf(yyout, "s%d", reg_no);
        return;
    }
    switch (reg_no)
    {
    case 11:
        fprintf(yyout, "fp");
        break;
    case 13:
        fprintf(yyout, "sp");
        break;
    case 14:
        fprintf(yyout, "lr");
        break;
    case 15:
        fprintf(yyout, "pc");
        break;
    default:
        fprintf(yyout, "r%d", reg_no);
        break;
    }
}

void MachineOperand::output()
{
    /* HINT：print operand
     * Example:
     * immediate num 1 -> print #1;
     * register 1 -> print r1;
     * lable addr_a -> print addr_a; */
    switch (this->type)
    {
    case IMM:
        fprintf(yyout, "#%d", this->val);
        break;
    case VREG:
    {
        if (fpu) // 这里如果是浮点虚拟，打印f，好调试
            fprintf(yyout, "f%d", this->reg_no);
        else
            fprintf(yyout, "v%d", this->reg_no);
    }
    break;
    case REG:
        PrintReg();
        break;
    case LABEL:
        if (this->label.substr(0, 2) == ".L") // 标签
            fprintf(yyout, "%s", this->label.c_str());
        else if (this->label.substr(0, 1) == "@") // 函数
            fprintf(yyout, "%s", this->label.c_str() + 1);
        else // 变量
            fprintf(yyout, "addr_%s_%d", this->label.c_str(), this->getParent()->getParent()->getParent()->getParent()->getLtorgNum());
    default:
        break;
    }
}

void MachineInstruction::PrintCond()
{
    // TODO
    switch (cond)
    {
    case EQ:
        fprintf(yyout, "eq");
        break;
    case NE:
        fprintf(yyout, "ne");
        break;
    case LT:
        fprintf(yyout, "lt");
        break;
    case LE:
        fprintf(yyout, "le");
        break;
    case GT:
        fprintf(yyout, "gt");
        break;
    case GE:
        fprintf(yyout, "ge");
        break;
    default:
        break;
    }
}

BinaryMInstruction::BinaryMInstruction(
    MachineBlock *p, int op,
    MachineOperand *dst, MachineOperand *src1, MachineOperand *src2,
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::BINARY;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
}

void BinaryMInstruction::output()
{
    // TODO:
    // Complete other instructions
    switch (this->op)
    {
    case BinaryMInstruction::ADD:
        fprintf(yyout, "\tadd ");
        break;
    case BinaryMInstruction::SUB:
        fprintf(yyout, "\tsub ");
        break;
    case BinaryMInstruction::MUL:
        fprintf(yyout, "\tmul ");
        break;
    case BinaryMInstruction::DIV:
        fprintf(yyout, "\tsdiv ");
        break;
    case BinaryMInstruction::AND:
        fprintf(yyout, "\tand ");
        break;
    case BinaryMInstruction::OR:
        fprintf(yyout, "\torr ");
        break;
    case BinaryMInstruction::VADD:
        fprintf(yyout, "\tvadd.f32 ");
        break;
    case BinaryMInstruction::VSUB:
        fprintf(yyout, "\tvsub.f32 ");
        break;
    case BinaryMInstruction::VMUL:
        fprintf(yyout, "\tvmul.f32 ");
        break;
    case BinaryMInstruction::VDIV:
        fprintf(yyout, "\tvdiv.f32 ");
        break;
    default:
        break;
    }
    this->PrintCond();
    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");
}

LoadMInstruction::LoadMInstruction(MachineBlock *p, int op,
                                   MachineOperand *dst, MachineOperand *src1, MachineOperand *src2,
                                   int cond)
{
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
    {
        this->use_list.push_back(src2);
        src2->setParent(this);
    }
}

void LoadMInstruction::output()
{
    switch (op)
    {
    case LDR:
        fprintf(yyout, "\tldr ");
        break;
    case VLDR:
        fprintf(yyout, "\tvldr.32 ");
        break;
    default:
        break;
    }
    this->def_list[0]->output();
    fprintf(yyout, ", ");

    // Load immediate num, eg: ldr r1, =8
    if (this->use_list[0]->isImm())
    {
        fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
        return;
    }

    // Load address
    if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
        fprintf(yyout, "[");

    this->use_list[0]->output();
    if (this->use_list.size() > 1)
    {
        fprintf(yyout, ", ");
        this->use_list[1]->output();
    }

    if (this->use_list[0]->isReg() || this->use_list[0]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

StoreMInstruction::StoreMInstruction(MachineBlock *p, int op,
                                     MachineOperand *src, MachineOperand *dst, MachineOperand *off,
                                     int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::STORE;
    this->op = op;
    this->cond = cond;
    use_list.push_back(src);
    use_list.push_back(dst);
    src->setParent(this);
    dst->setParent(this);
    if (off != nullptr)
    {
        use_list.push_back(off);
        off->setParent(this);
    }
}

void StoreMInstruction::output()
{
    // TODO
    switch (op)
    {
    case STR:
        fprintf(yyout, "\tstr ");
        break;
    case VSTR:
        fprintf(yyout, "\tvstr.32 ");
        break;
    default:
        break;
    }
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    // Store address
    if (this->use_list[1]->isReg() || this->use_list[1]->isVReg())
        fprintf(yyout, "[");
    this->use_list[1]->output();
    if (this->use_list.size() > 2)
    {
        fprintf(yyout, ", ");
        this->use_list[2]->output();
    }
    if (this->use_list[1]->isReg() || this->use_list[1]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
}

MovMInstruction::MovMInstruction(MachineBlock *p, int op,
                                 MachineOperand *dst, MachineOperand *src,
                                 int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::MOV;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

void MovMInstruction::output()
{
    // TODO
    switch (op)
    {
    case MOV:
        fprintf(yyout, "\tmov");
        break;
    case VMOV:
        fprintf(yyout, "\tvmov");
        break;
    case VMOV32:
        fprintf(yyout, "\tvmov.f32");
        break;
    case MVN: // 这个暂时用不到
    default:
        break;
    }
    PrintCond();
    fprintf(yyout, " ");
    def_list[0]->output();
    fprintf(yyout, ", ");
    use_list[0]->output();
    fprintf(yyout, "\n");
}

BranchMInstruction::BranchMInstruction(MachineBlock *p, int op,
                                       MachineOperand *dst,
                                       int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::BRANCH;
    this->op = op;
    this->cond = cond;
    this->use_list.push_back(dst);
    dst->setParent(this);
}

void BranchMInstruction::output()
{
    // TODO
    switch (op)
    {
    case B:
        fprintf(yyout, "\tb");
        break;
    case BX:
        fprintf(yyout, "\tbx");
        break;
    case BL:
        fprintf(yyout, "\tbl");
        break;
    default:
        break;
    }
    PrintCond();
    fprintf(yyout, " ");
    use_list[0]->output();
    fprintf(yyout, "\n");
}

CmpMInstruction::CmpMInstruction(MachineBlock *p, int op,
                                 MachineOperand *src1, MachineOperand *src2,
                                 int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::CMP;
    this->op = op;
    this->cond = cond;
    use_list.push_back(src1);
    use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);
}

void CmpMInstruction::output()
{
    // TODO
    switch (op)
    {
    case CMP:
        fprintf(yyout, "\tcmp ");
        break;
    case VCMP:
        fprintf(yyout, "\tvcmp.f32 ");
        break;
    default:
        break;
    }
    use_list[0]->output();
    fprintf(yyout, ", ");
    use_list[1]->output();
    fprintf(yyout, "\n");
}

StackMInstrcuton::StackMInstrcuton(MachineBlock *p, int op,
                                   std::vector<MachineOperand *> srcs,
                                   int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    for (auto operand : srcs)
    {
        use_list.push_back(operand);
        operand->setParent(this);
    }
}

void StackMInstrcuton::output()
{
    // TODO
    switch (op)
    {
    case POP:
        fprintf(yyout, "\tpop ");
        break;
    case PUSH:
        fprintf(yyout, "\tpush ");
        break;
    case VPOP:
        fprintf(yyout, "\tvpop ");
        break;
    case VPUSH:
        fprintf(yyout, "\tvpush ");
        break;
    default:
        break;
    }
    fprintf(yyout, "{");
    for (long unsigned int i = 0; i < use_list.size(); i++)
    {
        if (i != 0)
        {
            fprintf(yyout, ", ");
        }
        use_list[i]->output();
    }
    fprintf(yyout, "}\n");
}

VcvtMInstruction::VcvtMInstruction(MachineBlock *p, int op, MachineOperand *dst, MachineOperand *src, int cond)
{
    this->parent = p;
    this->type = MachineInstruction::VCVT;
    this->op = op;
    this->cond = cond;
    def_list.push_back(dst);
    use_list.push_back(src);
    dst->setParent(this);
    src->setParent(this);
}

void VcvtMInstruction::output()
{
    switch (op)
    {
    case FTS:
        fprintf(yyout, "\tvcvt.s32.f32 ");
        break;
    case STF:
        fprintf(yyout, "\tvcvt.f32.s32 ");
        break;
    default:
        break;
    }
    def_list[0]->output();
    fprintf(yyout, ", ");
    use_list[0]->output();
    fprintf(yyout, "\n");
}

VmrsMInstruction::VmrsMInstruction(MachineBlock *p, int cond)
{
    this->parent = p;
    this->type = MachineInstruction::VMRS;
    this->op = -1;
    this->cond = cond;
}

void VmrsMInstruction::output()
{
    // 这个指令目前只知道这一个用途，就是跟在vcmp后边传flag
    // 后期可以扩展
    fprintf(yyout, "\tvmrs APSR_nzcv, FPSCR\n");
}

MachineFunction::MachineFunction(MachineUnit *p, SymbolEntry *sym_ptr)
{
    this->parent = p;
    this->sym_ptr = sym_ptr;
    this->stack_size = 0;
    addSavedRegs(11);
    addSavedRegs(14);
};

void MachineBlock::insertBefore(MachineInstruction *insertee, MachineInstruction *pin)
{
    auto it = std::find(inst_list.begin(), inst_list.end(), pin);
    inst_list.insert(it, insertee);
}

void MachineBlock::insertAfter(MachineInstruction *insertee, MachineInstruction *pin)
{
    auto it = std::find(inst_list.begin(), inst_list.end(), pin);
    it++;
    inst_list.insert(it, insertee);
}

void MachineBlock::backPatch(std::vector<MachineOperand *> saved_regs)
{
    std::vector<MachineOperand *> rregs;
    std::vector<MachineOperand *> fregs;
    for (auto reg : saved_regs)
    {
        if (reg->isFReg())
            fregs.push_back(reg);
        else
            rregs.push_back(reg);
    }
    for (auto inst : unsure_insts)
    {
        if (inst->isStack())
        {
            if (inst->isVPOP())
            {
                if (fregs.empty()) // 如果是空的，把这条指令删掉
                {
                    eraseInst(inst);
                }
                else
                {
                    ((StackMInstrcuton *)inst)->setRegs(fregs);
                }
            }
            else if (inst->isPOP())
                ((StackMInstrcuton *)inst)->setRegs(rregs);
        }
        else if (inst->isLoad())
            ((LoadMInstruction *)inst)->setOff(saved_regs.size() * 4);
    }
}

void MachineBlock::output()
{
    fprintf(yyout, ".L%d:\n", this->no);
    unsigned long long int inst_num = 0;
    for (auto iter : inst_list)
    {
        iter->output();
        inst_num++;
        if (iter->isRet()) // 这里在每个函数返回语句处打印一个文字池
        {
            this->getParent()->getParent()->printLTORG();
        }
        else if (inst_num >= 256)
        { // 这里，每隔256条指令，打一个文字池
            inst_num = 0;
            int ltorg_num = this->getParent()->getParent()->getLtorgNum();
            fprintf(yyout, "\tb .LT%d\n", ltorg_num);
            this->getParent()->getParent()->printLTORG();
            fprintf(yyout, ".LT%d:\n", ltorg_num);
        }
    }
}

std::vector<MachineOperand *> MachineFunction::getAllSavedRegs()
{
    std::vector<MachineOperand *> saved_regs;
    for (auto reg_no : this->saved_regs)
    {
        if (reg_no < 15) // 通用寄存器
            saved_regs.push_back(new MachineOperand(MachineOperand::REG, reg_no));
        else if (reg_no > 15) // 专用寄存器
            saved_regs.push_back(new MachineOperand(MachineOperand::REG, reg_no, true));
    }
    return saved_regs;
}

std::vector<MachineOperand *> MachineFunction::getSavedRegs()
{
    std::vector<MachineOperand *> saved_regs;
    for (auto reg_no : this->saved_regs)
    {
        if (reg_no < 15)
            saved_regs.push_back(new MachineOperand(MachineOperand::REG, reg_no));
    }
    return saved_regs;
}

std::vector<MachineOperand *> MachineFunction::getSavedFRegs()
{
    std::vector<MachineOperand *> saved_regs;
    for (auto reg_no : this->saved_regs)
    {
        if (reg_no > 15)
            saved_regs.push_back(new MachineOperand(MachineOperand::REG, reg_no, true));
    }
    return saved_regs;
}

void MachineFunction::output()
{
    std::string funcName = this->sym_ptr->toStr() + "\0";
    const char *func_name = funcName.c_str() + 1;
    fprintf(yyout, "\t.global %s\n", func_name);
    fprintf(yyout, "\t.type %s , %%function\n", func_name);
    fprintf(yyout, "%s:\n", func_name);
    // TODO
    /* Hint:
     *  1. Save fp
     *  2. fp = sp
     *  3. Save callee saved register
     *  4. Allocate stack space for local variable */

    // Traverse all the block in block_list to print assembly code.
    auto entry = block_list[0];
    auto fp = new MachineOperand(MachineOperand::REG, 11);
    auto sp = new MachineOperand(MachineOperand::REG, 13);
    auto lr = new MachineOperand(MachineOperand::REG, 14);
    std::vector<MachineOperand *> save_regs = getSavedRegs();
    std::vector<MachineOperand *> save_fregs = getSavedFRegs();
    // save_regs.push_back(fp);
    // save_regs.push_back(lr);
    (new StackMInstrcuton(entry, StackMInstrcuton::PUSH, save_regs))->output();
    if (!save_fregs.empty())
        (new StackMInstrcuton(entry, StackMInstrcuton::VPUSH, save_fregs))->output();
    (new MovMInstruction(entry, MovMInstruction::MOV, fp, sp))->output();
    int stackSize = stack_size;
    auto stSize = new MachineOperand(MachineOperand::IMM, stackSize);
    if (AsmBuilder::isLegalImm(stackSize))
    {
        (new BinaryMInstruction(entry, BinaryMInstruction::SUB, sp, sp, stSize))->output();
    }
    else
    {
        if (stackSize & 0xff)
            (new BinaryMInstruction(entry, BinaryMInstruction::SUB, sp, sp, new MachineOperand(MachineOperand::IMM, stackSize & 0xff)))->output();
        if (stackSize & 0xff00)
            (new BinaryMInstruction(entry, BinaryMInstruction::SUB, sp, sp, new MachineOperand(MachineOperand::IMM, stackSize & 0xff00)))->output();
        if (stackSize & 0xff0000)
            (new BinaryMInstruction(entry, BinaryMInstruction::SUB, sp, sp, new MachineOperand(MachineOperand::IMM, stackSize & 0xff0000)))->output();
        if (stackSize & 0xff000000)
            (new BinaryMInstruction(entry, BinaryMInstruction::SUB, sp, sp, new MachineOperand(MachineOperand::IMM, stackSize & 0xff000000)))->output();
    }
    // 这一点是最坑的，按照龟腚来说arm里面的栈指针应该8字节对齐，如果不对齐的话，可能有问题
    // 然后这里就先算数右移三位再算数左移三位，8字节对齐一下，要不会发现浮点数有时候很奇怪
    fprintf(yyout, "\tlsr sp, sp, #3\n");
    fprintf(yyout, "\tlsl sp, sp, #3\n");
    for (auto iter : block_list)
    {
        iter->backPatch(getAllSavedRegs());
        iter->output();
    }
}

void MachineUnit::PrintGlobalDecl()
{
    // TODO:
    // You need to print global variable/const declarition code;
    // 先把const的和不是const的给分开
    std::vector<SymbolEntry *> commonVar;
    std::vector<SymbolEntry *> constVar;
    for (auto se : global_vars)
    {
        if (se->getType()->isInt() && ((IntType *)se->getType())->isConst() || se->getType()->isFloat() && ((FloatType *)se->getType())->isConst() || se->getType()->isArray() && ((ArrayType *)se->getType())->isConst())
        {
            constVar.push_back(se);
        }
        else
        {
            commonVar.push_back(se);
        }
    }
    // 不是const的放data区
    if (commonVar.size() > 0)
    {
        fprintf(yyout, ".data\n");
        for (auto se : commonVar)
        {
            if (se->getType()->isArray())
            {
                if (((IdentifierSymbolEntry *)se)->isInitial()) // 数组有初始化
                {
                    fprintf(yyout, ".global %s\n", se->toStr().c_str() + 1);
                    fprintf(yyout, ".size %s, %d\n", se->toStr().c_str() + 1, se->getType()->getSize() / 8);
                    fprintf(yyout, "%s:\n", se->toStr().c_str() + 1);
                    double *arrayValue = ((IdentifierSymbolEntry *)se)->getArrayValue();
                    int length = se->getType()->getSize() / 32;
                    if (((ArrayType *)se->getType())->getBaseType()->isFloat())
                    {
                        for (int i = 0; i < length; i++)
                        {
                            float value = (float)arrayValue[i];
                            uint32_t num = *((uint32_t *)&value);
                            fprintf(yyout, "\t.word %u\n", num);
                        }
                    }
                    else
                    {
                        for (int i = 0; i < length; i++)
                        {
                            fprintf(yyout, "\t.word %d\n", (int)arrayValue[i]);
                        }
                    }
                }
                else
                {
                    //.comm symbol, length:在bss段申请一段命名空间,该段空间的名称叫symbol, 长度为length.
                    // Ld 连接器在连接会为它留出空间.
                    fprintf(yyout, ".comm %s, %d\n", se->toStr().c_str() + 1, se->getType()->getSize() / 8);
                }
            }
            else
            {
                fprintf(yyout, ".global %s\n", se->toStr().c_str() + 1);
                fprintf(yyout, ".size %s, %d\n", se->toStr().c_str() + 1, se->getType()->getSize() / 8);
                fprintf(yyout, "%s:\n", se->toStr().c_str() + 1);
                if (se->getType()->isInt())
                    fprintf(yyout, "\t.word %d\n", (int)((IdentifierSymbolEntry *)se)->getValue());
                if (se->getType()->isFloat())
                {
                    float value = (float)((IdentifierSymbolEntry *)se)->getValue();
                    uint32_t num = *((uint32_t *)&value);
                    fprintf(yyout, "\t.word %u\n", num);
                }
            }
        }
    }
    // const的放只读区
    if (constVar.size() > 0)
    {
        fprintf(yyout, ".section .rodata\n");
        for (auto se : constVar)
        {
            if (se->getType()->isArray())
            {
                if (((IdentifierSymbolEntry *)se)->isInitial())
                {
                    fprintf(yyout, ".global %s\n", se->toStr().c_str() + 1);
                    fprintf(yyout, ".size %s, %d\n", se->toStr().c_str() + 1, se->getType()->getSize() / 8);
                    fprintf(yyout, "%s:\n", se->toStr().c_str() + 1);
                    double *arrayValue = ((IdentifierSymbolEntry *)se)->getArrayValue();
                    int length = se->getType()->getSize() / 32;
                    if (((ArrayType *)se->getType())->getBaseType()->isFloat())
                    {
                        for (int i = 0; i < length; i++)
                        {
                            float value = (float)arrayValue[i];
                            uint32_t num = *((uint32_t *)&value);
                            fprintf(yyout, "\t.word %u\n", num);
                        }
                    }
                    else
                    {
                        for (int i = 0; i < length; i++)
                        {
                            fprintf(yyout, "\t.word %d\n", (int)arrayValue[i]);
                        }
                    }
                }
                else
                {
                    //.comm symbol, length:在bss段申请一段命名空间,该段空间的名称叫symbol, 长度为length.
                    // Ld 连接器在连接会为它留出空间.
                    fprintf(yyout, ".comm %s, %d\n", se->toStr().c_str() + 1, se->getType()->getSize() / 8);
                }
            }
            else
            {
                fprintf(yyout, ".global %s\n", se->toStr().c_str() + 1);
                fprintf(yyout, ".size %s, %d\n", se->toStr().c_str() + 1, se->getType()->getSize() / 8);
                fprintf(yyout, "%s:\n", se->toStr().c_str() + 1);
                if (se->getType()->isInt())
                    fprintf(yyout, "\t.word %d\n", (int)((IdentifierSymbolEntry *)se)->getValue());
                if (se->getType()->isFloat())
                {
                    float value = (float)((IdentifierSymbolEntry *)se)->getValue();
                    uint32_t num = *((uint32_t *)&value);
                    fprintf(yyout, "\t.word %u\n", num);
                }
            }
        }
    }
}

void MachineUnit::printLTORG()
{
    // 打印文字池
    if (global_vars.size() > 0)
    {
        fprintf(yyout, "\n.LTORG\n");
        for (auto se : global_vars)
        {
            fprintf(yyout, "addr_%s_%d:\n", se->toStr().c_str() + 1, ltorg_num);
            fprintf(yyout, "\t.word %s\n", se->toStr().c_str() + 1);
        }
    }
    ltorg_num++;
}

void MachineUnit::output()
{
    // TODO
    /* Hint:
     * 1. You need to print global variable/const declarition code;
     * 2. Traverse all the function in func_list to print assembly code;
     * 3. Don't forget print bridge label at the end of assembly code!! */
    fprintf(yyout, "\t.arch armv8-a\n");
    fprintf(yyout, "\t.arch_extension crc\n");
    fprintf(yyout, "\t.arm\n");
    PrintGlobalDecl();
    fprintf(yyout, "\n\t.text\n");
    for (auto iter : func_list)
        iter->output();
    printLTORG();
}
