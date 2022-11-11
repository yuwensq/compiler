#ifndef __VALUE_H__
#define __VALUE_H__
#include <string>
#include <iostream>
#include <sstream>
struct Value
{
    enum
    {
        INT,
        FLOAT,
        VOID,
        BOOL,
        WRONGTYPE
    };
    char type;
    union
    {
        int intValue;
        float floatValue;
    };
    Value() {}
    Value(char t, int value)
    {
        type = t;
        intValue = value;
    }
    Value(char t, float value)
    {
        type = t;
        floatValue = value;
    }
    void asign(const Value &b)
    {
        if (this->type == INT && b.type == INT)
            this->intValue = b.intValue;
        else if (this->type == INT && b.type == FLOAT)
            this->intValue = (int)b.floatValue;
        else if (this->type == FLOAT && b.type == INT)
            this->floatValue = (float)b.intValue;
        else if (this->type == FLOAT && b.type == FLOAT)
            this->floatValue = b.floatValue;
        else
            this->type = WRONGTYPE;
        return;
    }
    Value operator+(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
        {
            value.type = WRONGTYPE;
        }
        else if (this->type == FLOAT && b.type == FLOAT)
        {
            value.type = FLOAT;
            value.floatValue = this->floatValue + b.floatValue;
        }
        else if (this->type == FLOAT && b.type == INT)
        {
            value.type = FLOAT;
            value.floatValue = this->floatValue + b.intValue;
        }
        else if (this->type == INT && b.type == FLOAT)
        {
            value.type = FLOAT;
            value.floatValue = this->intValue + b.floatValue;
        }
        else if (this->type == INT && b.type == INT)
        {
            value.type = INT;
            value.intValue = this->intValue + b.intValue;
        }
        return value;
    }
    Value operator-(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
        {
            value.type = WRONGTYPE;
        }
        else if (this->type == FLOAT && b.type == FLOAT)
        {
            value.type = FLOAT;
            value.floatValue = this->floatValue - b.floatValue;
        }
        else if (this->type == FLOAT && b.type == INT)
        {
            value.type = FLOAT;
            value.floatValue = this->floatValue - b.intValue;
        }
        else if (this->type == INT && b.type == FLOAT)
        {
            value.type = FLOAT;
            value.floatValue = this->intValue - b.floatValue;
        }
        else if (this->type == INT && b.type == INT)
        {
            value.type = INT;
            value.intValue = this->intValue - b.intValue;
        }
        return value;
    }
    Value operator*(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
        {
            value.type = WRONGTYPE;
        }
        else if (this->type == FLOAT && b.type == FLOAT)
        {
            value.type = FLOAT;
            value.floatValue = this->floatValue * b.floatValue;
        }
        else if (this->type == FLOAT && b.type == INT)
        {
            value.type = FLOAT;
            value.floatValue = this->floatValue * b.intValue;
        }
        else if (this->type == INT && b.type == FLOAT)
        {
            value.type = FLOAT;
            value.floatValue = this->intValue * b.floatValue;
        }
        else if (this->type == INT && b.type == INT)
        {
            value.type = INT;
            value.intValue = this->intValue * b.intValue;
        }
        return value;
    }
    Value operator/(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
        {
            value.type = WRONGTYPE;
        }
        else if (this->type == FLOAT && b.type == FLOAT)
        {
            value.type = FLOAT;
            value.floatValue = this->floatValue / b.floatValue;
        }
        else if (this->type == FLOAT && b.type == INT)
        {
            value.type = FLOAT;
            value.floatValue = this->floatValue / b.intValue;
        }
        else if (this->type == INT && b.type == FLOAT)
        {
            value.type = FLOAT;
            value.floatValue = this->intValue / b.floatValue;
        }
        else if (this->type == INT && b.type == INT)
        {
            value.type = INT;
            value.intValue = this->intValue / b.intValue;
        }
        return value;
    }
    Value operator%(const Value &b)
    {
        Value value;
        if (this->type == INT && b.type == INT)
        {
            value.type = INT;
            value.intValue = this->intValue % b.intValue;
        }
        else
            value.type = WRONGTYPE;
        return value;
    }
    Value operator<(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
            value.type = WRONGTYPE;
        else
        {
            value.type = INT;
            if (this->type == FLOAT && b.type == FLOAT)
                value.intValue = this->floatValue < b.floatValue;
            else if (this->type == FLOAT && b.type == INT)
                value.intValue = this->floatValue < b.intValue;
            else if (this->type == INT && b.type == FLOAT)
                value.intValue = this->intValue < b.floatValue;
            else if (this->type == INT && b.type == INT)
                value.intValue = this->intValue < b.intValue;
        }
        return value;
    }
    Value operator<=(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
            value.type = WRONGTYPE;
        else
        {
            value.type = INT;
            if (this->type == FLOAT && b.type == FLOAT)
                value.intValue = this->floatValue <= b.floatValue;
            else if (this->type == FLOAT && b.type == INT)
                value.intValue = this->floatValue <= b.intValue;
            else if (this->type == INT && b.type == FLOAT)
                value.intValue = this->intValue <= b.floatValue;
            else if (this->type == INT && b.type == INT)
                value.intValue = this->intValue <= b.intValue;
        }
        return value;
    }
    Value operator>(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
            value.type = WRONGTYPE;
        else
        {
            value.type = INT;
            if (this->type == FLOAT && b.type == FLOAT)
                value.intValue = this->floatValue > b.floatValue;
            else if (this->type == FLOAT && b.type == INT)
                value.intValue = this->floatValue > b.intValue;
            else if (this->type == INT && b.type == FLOAT)
                value.intValue = this->intValue > b.floatValue;
            else if (this->type == INT && b.type == INT)
                value.intValue = this->intValue > b.intValue;
        }
        return value;
    }
    Value operator>=(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
            value.type = WRONGTYPE;
        else
        {
            value.type = INT;
            if (this->type == FLOAT && b.type == FLOAT)
                value.intValue = this->floatValue >= b.floatValue;
            else if (this->type == FLOAT && b.type == INT)
                value.intValue = this->floatValue >= b.intValue;
            else if (this->type == INT && b.type == FLOAT)
                value.intValue = this->intValue >= b.floatValue;
            else if (this->type == INT && b.type == INT)
                value.intValue = this->intValue >= b.intValue;
        }
        return value;
    }
    Value operator==(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
            value.type = WRONGTYPE;
        else
        {
            value.type = INT;
            if (this->type == FLOAT && b.type == FLOAT)
                value.intValue = this->floatValue == b.floatValue;
            else if (this->type == FLOAT && b.type == INT)
                value.intValue = this->floatValue == b.intValue;
            else if (this->type == INT && b.type == FLOAT)
                value.intValue = this->intValue == b.floatValue;
            else if (this->type == INT && b.type == INT)
                value.intValue = this->intValue == b.intValue;
        }
        return value;
    }
    Value operator!=(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
            value.type = WRONGTYPE;
        else
        {
            value.type = INT;
            if (this->type == FLOAT && b.type == FLOAT)
                value.intValue = this->floatValue != b.floatValue;
            else if (this->type == FLOAT && b.type == INT)
                value.intValue = this->floatValue != b.intValue;
            else if (this->type == INT && b.type == FLOAT)
                value.intValue = this->intValue != b.floatValue;
            else if (this->type == INT && b.type == INT)
                value.intValue = this->intValue != b.intValue;
        }
        return value;
    }
    Value operator&&(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
            value.type = WRONGTYPE;
        else
        {
            value.type = INT;
            if (this->type == FLOAT && b.type == FLOAT)
                value.intValue = this->floatValue && b.floatValue;
            else if (this->type == FLOAT && b.type == INT)
                value.intValue = this->floatValue && b.intValue;
            else if (this->type == INT && b.type == FLOAT)
                value.intValue = this->intValue && b.floatValue;
            else if (this->type == INT && b.type == INT)
                value.intValue = this->intValue && b.intValue;
        }
        return value;
    }
    Value operator||(const Value &b)
    {
        Value value;
        if (this->type == VOID || b.type == VOID || this->type == WRONGTYPE || b.type == WRONGTYPE)
            value.type = WRONGTYPE;
        else
        {
            value.type = INT;
            if (this->type == FLOAT && b.type == FLOAT)
                value.intValue = this->floatValue || b.floatValue;
            else if (this->type == FLOAT && b.type == INT)
                value.intValue = this->floatValue || b.intValue;
            else if (this->type == INT && b.type == FLOAT)
                value.intValue = this->intValue || b.floatValue;
            else if (this->type == INT && b.type == INT)
                value.intValue = this->intValue || b.intValue;
        }
        return value;
    }
    Value operator-()
    {
        Value value;
        if (this->type == VOID || this->type == WRONGTYPE)
            value.type = WRONGTYPE;
        value.type = this->type;
        if (this->type == INT)
            value.intValue = -this->intValue;
        else if (this->type == FLOAT)
            value.floatValue = -this->floatValue;
        return value;
    }
    Value operator!()
    {
        Value value;
        if (this->type == VOID || this->type == WRONGTYPE)
            value.type = WRONGTYPE;
        value.type = INT;
        if (this->type == INT)
            value.intValue = !this->intValue;
        else if (this->type == FLOAT)
            value.floatValue = !this->floatValue;
        return value;
    }
    std::string toStr()
    {
        std::ostringstream buffer;
        if (type == INT)
            buffer << intValue;
        else if (type == FLOAT)
            buffer << floatValue;
        return buffer.str();
    }
};
#endif