//
// Created by guoqi on 10/10/15.
//

#include <cassert>
#include "Code.h"

uint64_t Instruction::opCode ()
{
    return __instruction >> 56;
}

uint64_t Instruction::dst ()
{
    return (__instruction & 0x0000ffffff000000) >> 24;
}

uint64_t Instruction::src ()
{
    return (__instruction & 0x0000000000ffffff);
}

uint64_t Instruction::size ()
{
    return (__instruction & 0x00ff000000000000) >> 48;
}

uint64_t Instruction::mode ()
{
    return (__instruction & 0x00ff000000000000) >> 48;
}

uint64_t Instruction::targetAddress ()
{
    return (__instruction & 0x0000ffffffffffff);
}

string Instruction::toString ()
{
    // TODO
}

void Instruction::toBinary (char *buf)
{
    // TODO
}



Instruction InstructionFactory::createInstruction (uint64_t op)
{
    assert (op & ZeroOperandMask);
    return Instruction(op << 56);
}

Instruction InstructionFactory::createInstruction (uint64_t op, uint64_t targetAddress)
{
    assert (op & SingleOperandMask);
    return (op << 56) | (0x0000ffffffffffff & targetAddress);
}

Instruction InstructionFactory::createInstruction (uint64_t op, uint64_t mode, uint64_t src, uint64_t dst)
{
    assert (op & DoubleOperandMask);
    return (op << 56) | ((mode << 48) & 0x00ff000000000000) | ((src << 24) & 0x0000ffffff000000) | (dst & 0x0000000000ffffff);
}



Code::Code (const Code &code)
{
    __codeArea_size = code.__codeArea_size;
    __dataArea_size = code.__dataArea_size;
    for (int i=0; i<__codeArea_size; i++) {
        __codeArea[i] = code.__codeArea[i];
    }
}

void Code::insertInstruction (uint64_t address, Instruction inc)
{
    assert (address < CODESIZE);
    __codeArea[address] = inc;
    __codeArea_size += 1;
}
