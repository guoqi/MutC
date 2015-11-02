//
// Created by guoqi on 10/10/15.
//

#include <cassert>
#include "MemoryMap.h"

uint64_t sizeOf (Type *type)
{
    uint64_t sum = 0;
    switch (type->info ())
    {
        case TypeInfo::Integer:
        case TypeInfo::Real:
        case TypeInfo::Pointer:
            return 8;
        case TypeInfo::String:
            // TODO
            return 8;
        case TypeInfo::Array:
            return static_cast<ArrayType *>(type)->size () * sizeOf (type->next ());
        case TypeInfo::Struct:
            for (auto & i : static_cast<StructType *>(type)->symEntry ()->memlist.table ())
            {
                sum += sizeOf (static_pointer_cast <VarEntry> (i)->typeInfo ());
            }
            return sum;
        default:
            return 0;
    }
}

uint64_t MemoryMap::mmap (uint64_t size)
{
    assert (__address + size < __limit);
    uint64_t tmp = __address;
    __address += size;
    return tmp;
}

uint64_t MemoryMap::mmap (Type *type)
{
    uint64_t size = sizeOf (type);
    return mmap (size);
}


// 栈地址分配器分配得到的是一个相对于bp的偏移量
uint64_t StackMemoryMap::mmap (uint64_t size)
{
    assert (__sp - size >= 0);
    uint64_t tmp = __bp_stack.top () - __sp;
    __sp -= size;
    return tmp;
}

uint64_t StackMemoryMap::mmap (Type *type)
{
    return mmap(sizeOf (type));
}

void StackMemoryMap::push ()
{
    __bp_stack.push (__sp);
}

void StackMemoryMap::pop ()
{
    __sp = __bp_stack.top ();
    __bp_stack.pop ();
}

uint64_t CodeMemoryMap::mmap ()
{
    return MemoryMap::mmap(1);
}
