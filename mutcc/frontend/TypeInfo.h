//
// Created by guoqi on 9/24/15.
//

#ifndef MUTCC_TYPEINFO_H
#define MUTCC_TYPEINFO_H

#include <memory>
#include <string>
#include <cctype>
#include "SymbolTable.h"

using namespace std;

enum class TypeInfo {
/*
 * Semantics requirement: Only Pointer type and array type can point to another type.
 */
    // basic atomic type
    Integer,
    Real,
    String,
    Boolean,
    Pointer,
    // basic aggregation type
    Array,
    Struct
};

class Type
{
public:
    Type(TypeInfo info): __info(info) {}
    virtual ~Type() { if (__next) delete __next; }

    inline Type * next() const { return __next; }
    inline Type * next(Type * next) { __next = next; }

    inline TypeInfo info() const { return __info; }

protected:
    Type *      __next;
    TypeInfo    __info;
};

class AtomicType: public Type
{
public:
    AtomicType(TypeInfo info): Type(info) {}
    virtual ~AtomicType () {}
};

class ArrayType: public Type
{
public:
    ArrayType(uint64_t size): Type(TypeInfo::Array), __size(size) {}
    virtual ~ArrayType () {}

    inline uint64_t size() { return __size; }

private:
    uint64_t         __size;
};

class StructType: public Type
{
public:
    StructType(string name): Type(TypeInfo::Struct), __name(name) {}
    virtual ~StructType () {}

    inline string name() { return __name; }

private:
    string      __name;
};



#endif //MUTCC_TYPEINFO_H
