//
// Created by guoqi on 11/2/15.
//

#include "backend/Instruction.h"
#include "backend/Size.h"
#include "VirtualMachine.h"

VM::VM (uint64_t main_addr, uint64_t code_size, uint64_t data_size)
    : __pc(main_addr)
{
    __code = new Instruction[code_size];
    __data = new uint64_t[data_size];
    __stack = new uint64_t[STACKSIZE];
}

VM::~VM ()
{
    delete __code;
    delete __data;
    delete __stack;
}

void VM::execute ()
{
    Instruction inc;
    while ((inc = fetchInc ()))
    {
        switch (inc.opCode ()) {
            case HALT:
                return;
            case RET:
                // TODO
                break;
            case PUSHSP:
                // TODO
                break;
            case POPSP:
                // TODO
                break;
            case CALL:
                // TODO
                break;
            case JZ:
                if (data (inc.cond (), FROMSTACK) == 0) {
                    jmpTo (inc.targetAddress ());
                }
                break;
            case JNZ:
                if (data (inc.cond (), FROMSTACK) != 0) {
                    jmpTo (inc.targetAddress ());
                }
                break;
            case JMP:
                jmpTo (inc.targetAddress ());
                break;
            case MOV:
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK);
                break;
            case ADD:
                break;
            case SUB:
                break;
            case MUL:
                break;
            case DIV:
                break;
            case MOD:
                break;
            case LAND:
                break;
            case LOR:
                break;
            case LNOT:
                break;
            case CG:
                break;
            case CL:
                break;
            case CGE:
                break;
            case CLE:
                break;
            case CE:
                break;
            case CNE:
                break;
        }
    }
}

Instruction VM::fetchInc ()
{
    return __code[__pc];
}

uint64_t & VM::data (uint64_t addr, int flag)
{
    if (flag == FROMDATA) {
        return __data[addr];
    }
    else if (flag == FROMSTACK) {
        return __stack[addr];
    }
    else {
        return -1;
    }
}

void VM::jmpTo (uint64_t addr)
{
    __pc = addr;
}

void VM::nextPC ()
{
    ++__pc;
}


VM * initVM(ifstream & file)
{
    // TODO
}
