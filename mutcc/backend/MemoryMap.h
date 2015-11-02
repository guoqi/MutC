//
// Created by guoqi on 10/10/15.
//

#ifndef MUTCC_MEMORYMAP_H
#define MUTCC_MEMORYMAP_H

#include <cstdint>
#include <stack>
#include "Size.h"
#include "../frontend/TypeInfo.h"

using namespace std;

uint64_t sizeOf (Type *type);

class MemoryMap
{
public:
    MemoryMap(uint64_t limit): __address(0), __limit(limit) {}

    // assert address < limit
    virtual uint64_t mmap(uint64_t size);

    virtual uint64_t mmap (Type *type);

    inline uint64_t address() { return __address; }

private:
    uint64_t        __address;
    uint64_t        __limit;
};

class StackMemoryMap: public MemoryMap
{
public:
    StackMemoryMap(): MemoryMap(STACKSIZE), __sp(STACKSIZE) {}

    virtual uint64_t mmap(uint64_t size);

    virtual uint64_t mmap (Type *type);

    void push();
    void pop();

    inline uint64_t bp() { return __bp_stack.top (); }
    inline uint64_t sp() { return __sp; }

private:
    uint64_t        __sp; // top of stack frame
    stack<uint64_t> __bp_stack; // bottom of stack frame
};

class DataMemoryMap: public MemoryMap
{
public:
    DataMemoryMap(): MemoryMap(DATASIZE) {}
};

class CodeMemoryMap: public MemoryMap
{
public:
    CodeMemoryMap(): MemoryMap(CODESIZE) {}

    inline uint64_t curAddr() { return address (); }

    uint64_t mmap();
};

#endif //MUTCC_MEMORYMAP_H
