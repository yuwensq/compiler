#ifndef __TYPE_H__
#define __TYPE_H__
#include <iostream>
#include <string>
#include <vector>

class Type {
   private:
    int kind;

   protected:
    enum { INT, VOID, FUNC, STRING };
    int size;

   public:
    Type(int kind, int size = 0) : kind(kind), size(size){};
    virtual ~Type(){};
    virtual std::string toStr() = 0;
    bool isInt() const { return kind == INT; };
    bool isVoid() const { return kind == VOID; };
    bool isFunc() const { return kind == FUNC; };
    bool isString() const { return kind == STRING; };
    int getSize() const { return size; };
};

class IntType : public Type {
   private:
    bool constant;

   public:
    IntType(int size, bool constant = false)
        : Type(Type::INT, size), constant(constant){};
    std::string toStr();
    bool isConst() const { return constant; };
};

class VoidType : public Type {
   public:
    VoidType() : Type(Type::VOID){};
    std::string toStr();
};

class FunctionType : public Type {
   private:
    Type* returnType;
    std::vector<Type*> paramsType;

   public:
    FunctionType(Type* returnType, std::vector<Type*> paramsType)
        : Type(Type::FUNC), returnType(returnType), paramsType(paramsType){};
    void setParamsType(std::vector<Type*> paramsType) {
        this->paramsType = paramsType;
    };
    std::string toStr();
};

class StringType : public Type {
   private:
    int length;

   public:
    StringType(int length) : Type(Type::STRING), length(length){};
    int getLength() const { return length; };
    std::string toStr();
};

class TypeSystem {
   private:
    static IntType commonInt;
    static VoidType commonVoid;
    static IntType commonConstInt;

   public:
    static Type* intType;
    static Type* voidType;
    static Type* constIntType;
};

#endif
