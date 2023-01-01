#include "MachineCode.h"
#include "Type.h"
#include <iostream>
extern FILE *yyout;

MachineOperand::MachineOperand(int tp, int val)
{
    this->type = tp;
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
        fprintf(yyout, "v%d", this->reg_no);
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
            fprintf(yyout, "addr_%s%d", this->label.c_str(), this->getParent()->getParent()->getParent()->getParent()->getLtorgNum());
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

LoadMInstruction::LoadMInstruction(MachineBlock *p,
                                   MachineOperand *dst, MachineOperand *src1, MachineOperand *src2,
                                   int cond)
{
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = -1;
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
    fprintf(yyout, "\tldr ");
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

StoreMInstruction::StoreMInstruction(MachineBlock *p,
                                     MachineOperand *src, MachineOperand *dst, MachineOperand *off,
                                     int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::STORE;
    this->op = -1;
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
    fprintf(yyout, "\tstr ");
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
    fprintf(yyout, "\tmov");
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

CmpMInstruction::CmpMInstruction(MachineBlock *p,
                                 MachineOperand *src1, MachineOperand *src2,
                                 int cond)
{
    // TODO
    this->parent = p;
    this->type = MachineInstruction::CMP;
    this->op = -1;
    this->cond = cond;
    use_list.push_back(src1);
    use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);
}

void CmpMInstruction::output()
{
    // TODO
    fprintf(yyout, "\tcmp ");
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

MachineFunction::MachineFunction(MachineUnit *p, SymbolEntry *sym_ptr)
{
    this->parent = p;
    this->sym_ptr = sym_ptr;
    this->stack_size = 0;
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

void MachineBlock::backPatch(std::vector<MachineOperand *> saved_regs, int stack_size)
{
    for (auto inst : unsure_insts)
    {
        if (inst->isStack())
            ((StackMInstrcuton *)inst)->setRegs(saved_regs);
        else if (inst->isLoad())
            ((LoadMInstruction *)inst)->setOff(saved_regs.size() * 4);
        else if (inst->isBinary())
        {
            int stackSize = stack_size;
            auto father_block = inst->getParent();
            while (stackSize > 16384)
            {
                father_block->insertAfter(new BinaryMInstruction(father_block,
                                                                 BinaryMInstruction::ADD,
                                                                 new MachineOperand(MachineOperand::REG, 13),
                                                                 new MachineOperand(MachineOperand::REG, 13),
                                                                 new MachineOperand(MachineOperand::IMM, 16384)),
                                          inst);
                stackSize -= 16384;
            }
            int ele = 16384;
            while (stackSize > 256)
            {
                if (stackSize >= ele)
                {
                    father_block->insertAfter(new BinaryMInstruction(father_block,
                                                                     BinaryMInstruction::ADD,
                                                                     new MachineOperand(MachineOperand::REG, 13),
                                                                     new MachineOperand(MachineOperand::REG, 13),
                                                                     new MachineOperand(MachineOperand::IMM, ele)),
                                              inst);
                    stackSize -= ele;
                }
                ele /= 2;
            }
            ((BinaryMInstruction *)inst)->setStackSize(stackSize);
        }
    }
}

void MachineBlock::output()
{
    fprintf(yyout, ".L%d:\n", this->no);
    for (auto iter : inst_list)
    {
        iter->output();
        if (iter->isRet()) // 这里在每个函数返回语句处打印一个文字池
        {
            this->getParent()->getParent()->printLTORG();
        }
    }
}

std::vector<MachineOperand *> MachineFunction::getSavedRegs()
{
    std::vector<MachineOperand *> saved_regs;
    for (auto reg_no : this->saved_regs)
    {
        saved_regs.push_back(new MachineOperand(MachineOperand::REG, reg_no));
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
    save_regs.push_back(fp);
    save_regs.push_back(lr);
    int stackSize = stack_size;
    // 关于arm的立即数大小，很奇怪
    while (stackSize > 16384)
    {
        // 由于立即数有大小限制，好像最大一次可以减16384，太大的分多次减
        entry->insertFront(new BinaryMInstruction(entry, BinaryMInstruction::SUB, sp, sp, new MachineOperand(MachineOperand::IMM, 16384)));
        stackSize -= 16384;
    }
    // 剩下的一定小于等于16384，但是好像对于不是2的幂次的数，立即数最大能用255，这里我们拆成2的幂次
    int ele = 16384;
    while (stackSize > 256)
    {
        if (stackSize >= ele)
        {
            entry->insertFront(new BinaryMInstruction(entry, BinaryMInstruction::SUB, sp, sp, new MachineOperand(MachineOperand::IMM, ele)));
            stackSize -= ele;
        }
        ele /= 2;
    }
    if (stackSize)
    {
        entry->insertFront(new BinaryMInstruction(entry, BinaryMInstruction::SUB, sp, sp, new MachineOperand(MachineOperand::IMM, stackSize)));
    }
    entry->insertFront(new MovMInstruction(entry, MovMInstruction::MOV, fp, sp));
    entry->insertFront(new StackMInstrcuton(entry, StackMInstrcuton::PUSH, save_regs));
    unsigned long long inst_num = 0;
    for (auto iter : block_list)
    {
        iter->backPatch(save_regs, stack_size);
        iter->output();
        inst_num += iter->getInsts().size();
        // if (inst_num >= 256) // 这里本来想256条指令打印一个文字池，不过效果不是很好
        // {
        //     this->parent->printLTORG();
        // }
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
        if ((se->getType()->isInt() && ((IntType *)se->getType())->isConst()) || se->getType()->isArray() && ((ArrayType *)se->getType())->isConst())
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
                    int *arrayValue = ((IdentifierSymbolEntry *)se)->getArrayValue();
                    int length = se->getType()->getSize() / 32;
                    for (int i = 0; i < length; i++)
                    {
                        fprintf(yyout, "\t.word %d\n", arrayValue[i]);
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
                fprintf(yyout, "\t.word %d\n", ((IdentifierSymbolEntry *)se)->getValue());
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
                    int *arrayValue = ((IdentifierSymbolEntry *)se)->getArrayValue();
                    int length = se->getType()->getSize() / 32;
                    for (int i = 0; i < length; i++)
                    {
                        fprintf(yyout, "\t.word %d\n", arrayValue[i]);
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
                fprintf(yyout, "\t.word %d\n", ((IdentifierSymbolEntry *)se)->getValue());
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
            fprintf(yyout, "addr_%s%d:\n", se->toStr().c_str() + 1, ltorg_num);
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
