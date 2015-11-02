//
// Created by guoqi on 10/10/15.
//

#include <cassert>
#include <sstream>
#include "Code.h"

uint64_t Instruction::opCode ()
{
    return __instruction >> 56;
}

uint64_t Instruction::dst ()
{
    return (__instruction & 0x0000FFFF00000000) >> 32;
}

uint64_t Instruction::src1 ()
{
    return (__instruction & 0x00000000FFFF0000) >> 16;
}

uint64_t Instruction::src2 ()
{
    return (__instruction & 0x000000000000FFFF);
}

uint64_t Instruction::size ()
{
    return (__instruction & 0x000F000000000000) >> 48;
}

uint64_t Instruction::mode ()
{
    return (__instruction & 0x00C0000000000000) >> 54;
}

uint64_t Instruction::flag ()
{
    return (__instruction & 0x0030000000000000) >> 52;
}

uint64_t Instruction::cond ()
{
    return (__instruction & 0x0000FFFF00000000) >> 32;
}

uint64_t Instruction::targetAddress ()
{
    return (__instruction & 0x00000000FFFFFFFF);
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

Instruction InstructionFactory::createInstruction (uint64_t op, uint64_t mode, uint64_t size, uint64_t cond, uint64_t targetAddress)
{
    assert (op & SingleOperandMask);
    return Instruction((op << 56) | ((mode << 54) & 0x00C0000000000000) | ((size << 48) & 0x000F000000000000) | (0x00000000FFFFFFFF & targetAddress));
}

Instruction InstructionFactory::createInstruction (uint64_t op, uint64_t mode, uint64_t size, uint64_t dst, uint64_t src1, uint64_t src2)
{
    assert (op & DoubleOperandMask);
    return Instruction((op << 56) | ((mode << 54) & 0x00C0000000000000) | ((size << 48) & 0x000F0000000000) | ((dst << 32) & 0x0000FFFF00000000)
                       | ((src1 << 16) & 0x00000000FFFF0000) | (src2 & 0x000000000000FFFF));
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

string Code::toString ()
{
    stringstream ss;
    for (int i=0; i<__codeArea_size; i++) {
        ss << __codeArea[i].toString () << endl;
    }
    return ss.str ();
}
