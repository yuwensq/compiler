#ifndef __TYPE_H__
#define __TYPE_H__
#include <vector>
#include <string>

class Type
{
private:
    int kind;

protected:
    enum
    {
        INT,
        BOOL,
        VOID,
        FUNC,
        PTR,
        ARRAY
    };
    int size;

public:
    Type(int kind, int size = 0) : kind(kind), size(size){};
    virtual ~Type(){};
    virtual std::string toStr() = 0;
    bool isInt() const { return kind == INT; };
    bool isBool() const { return kind == BOOL; };
    bool isVoid() const { return kind == VOID; };
    bool isFunc() const { return kind == FUNC; };
    bool isPtr() const { return kind == PTR; };
    bool isArray() const { return kind == ARRAY; };
    int getKind() const { return kind; };
    int getSize() const { return size; };
};

class IntType : public Type
{
private:
    bool constant;

public:
    IntType(int size, bool constant = false) : Type(size == 1 ? Type::BOOL : Type::INT, size), constant(constant){};
    std::string toStr();
    bool isConst() const { return constant; };
};

class VoidType : public Type
{
public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class FunctionType : public Type
{
private:
    Type *returnType;
    std::vector<Type *> paramsType;

public:
    FunctionType(Type *returnType, std::vector<Type *> paramsType) : Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    Type *getRetType() { return returnType; };
    void setParamsType(std::vector<Type *> paramsType)
    {
        this->paramsType = paramsType;
    };
    std::vector<Type *> getParamsType() { return paramsType; };
    std::string toStr();
};

class PointerType : public Type
{
private:
    Type *valueType;

public:
    PointerType(Type *valueType) : Type(Type::PTR)
    {
        this->valueType = valueType;
        this->size = 32;
    };
    std::string toStr();
    Type *getType() const { return valueType; };
};

class TypeSystem
{
private:
    static IntType commonInt;
    static IntType commonBool;
    static VoidType commonVoid;
    static IntType commonConstInt;

public:
    static Type *intType;
    static Type *voidType;
    static Type *boolType;
    static Type *constIntType;
};

class ArrayType : public Type
{
private:
    // 数组的索引范围
    std::vector<int> indexs;
    Type *baseType;

public:
    ArrayType(std::vector<int> indexs, Type *baseType = TypeSystem::intType) : Type(Type::ARRAY), indexs(indexs), baseType(baseType)
    {
        this->size = 1;
        for (auto index : indexs)
        {
            this->size *= index;
        }
        this->size *= 32;
    };
    std::string toStr();
    std::vector<int> getIndexs() { return indexs; };
    bool isConst() { return ((IntType *)baseType)->isConst(); };
};

#endif