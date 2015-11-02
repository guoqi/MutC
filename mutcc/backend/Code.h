//
// Created by guoqi on 10/10/15.
//

#ifndef MUTCC_CODE_H
#define MUTCC_CODE_H

#include <cstdint>
#include <string>
#include <vector>
#include "Size.h"

#define ZeroOperandMask 0x00
#define SingleOperandMask 0x40
#define DoubleOperandMask 0x80

using namespace std;

class Instruction
{
public:
    Instruction() {}
    Instruction(uint64_t instruction): __instruction(instruction) {}

    // return operate code
    uint64_t opCode ();

    // return destation number address
    uint64_t dst();

    // return source 1 number address
    uint64_t src1();

    // return source 2 number address
    uint64_t src2();

    // return size
    uint64_t size ();

    // return operate mode
    uint64_t mode ();

    uint64_t flag ();

    // return condition variable address
    uint64_t cond();

    // return target code address
    uint64_t targetAddress();

    string toString();
    void toBinary(char * buf);

    inline uint64_t instruction() { return __instruction; }

private:
    uint64_t        __instruction;
};

class InstructionFactory
{
public:
    Instruction createInstruction (uint64_t op);
    Instruction createInstruction (uint64_t op, uint64_t mode, uint64_t size, uint64_t cond, uint64_t targetAddress);
    Instruction createInstruction (uint64_t op, uint64_t mode, uint64_t size, uint64_t dst, uint64_t src1, uint64_t src2);
};

class Code
{
public:
    Code(): __main_addr(-1) {}

    // copy constructor
    Code(const Code & code);

    void insertInstruction(uint64_t address, Instruction inc);

    inline uint64_t codeSize() { return __codeArea_size; }
    inline uint64_t dataSize() { return __dataArea_size; }
    inline void dataSize(uint64_t size) { __dataArea_size = size; }
    inline uint64_t mainAddr() { return __main_addr; }
    inline void mainAddr(uint64_t main_addr) { __main_addr = main_addr; }

    string toString();

private:
    Instruction         __codeArea[CODESIZE];
    uint64_t            __codeArea_size;
    uint64_t            __dataArea_size;
    uint64_t            __main_addr;
};

#endif //MUTCC_CODE_H
