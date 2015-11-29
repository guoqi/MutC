//
// Created by guoqi on 11/2/15.
//

#include <iostream>
#include <sstream>
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
    while (1)
    {
        inc = fetchInc ();
        // cout << inc.opCode () << endl;
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
            // cout << "debug cond: " << inc.cond () << "\t";
            // cout << "Address: " << inc.targetAddress () << endl;
            if (data (inc.cond (), FROMSTACK) == 0) {
                jmpTo (inc.targetAddress ());
                continue;
            }
            break;
        case JNZ:
            if (data (inc.cond (), FROMSTACK) != 0) {
                jmpTo (inc.targetAddress ());
                continue;
            }
            break;
        case JMP:
            jmpTo (inc.targetAddress ());
            continue;
        case PNT:
            cout << data(inc.cond (), FROMSTACK) << endl;
            break;
        case MOV:
            // cout << "mov dst: " << inc.dst () << "\t";
            if (inc.mode () == 0x00)
            {
                // cout << "mov src1 data: " << data(inc.src1 (), FROMSTACK) << endl;
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK);
            }
            else if (inc.mode () == 0x02)
            {
                // cout << "mov src1: " << inc.src1 () << endl;
                data (inc.dst (), FROMSTACK) = inc.src1 ();
            }
            break;
        case ADD:
            // cout << "dst: " << data (inc.dst (), FROMSTACK) << "\t";
            if (inc.mode () == 0x00)
            {
                // cout << "src1 data: " << data(inc.src1 (), FROMSTACK) << "\t";
                // cout << "src2 data: " << data(inc.src2 (), FROMSTACK) << endl;
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) + data (inc.src2 (), FROMSTACK);
            }
            else if (inc.mode () == 0x01)
            {
                // cout << "src1 data: " << data(inc.src1 (), FROMSTACK) << "\t";
                // cout << "src2: " << inc.src2 () << endl;
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) + inc.src2 ();
            }
            else if (inc.mode () == 0x02)
            {
                // cout << "src1: " << inc.src1 () << "\t";
                // cout << "src2 data: " << data (inc.src2 (), FROMSTACK) << endl;
                data (inc.dst (), FROMSTACK) = inc.src1 () + data (inc.src2 (), FROMSTACK);
            }
            else
            {
                // cout << "src1: " << inc.src1 () << "\t";
                // cout << "src2: " << inc.src2 () << endl;
                data (inc.dst (), FROMSTACK) = inc.src1 () + inc.src2 ();
            }
            // cout << "after dst: " << data(inc.dst (), FROMSTACK) << endl;
            break;
        case SUB:
            if (inc.mode () == 0x00)
            {
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) - data (inc.src2 (), FROMSTACK);
                // cout << "result: " << data (inc.dst (), FROMSTACK) << endl;
            }
            else if (inc.mode () == 0x01)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) - inc.src2 ();
            else if (inc.mode () == 0x02)
                data (inc.dst (), FROMSTACK) = inc.src1 () - data (inc.src2 (), FROMSTACK);
            else
                data (inc.dst (), FROMSTACK) = inc.src1 () - inc.src2 ();
            break;
        case MUL:
            if (inc.mode () == 0x00)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) * data (inc.src2 (), FROMSTACK);
            else if (inc.mode () == 0x01)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) * inc.src2 ();
            else if (inc.mode () == 0x02)
                data (inc.dst (), FROMSTACK) = inc.src1 () * data (inc.src2 (), FROMSTACK);
            else
                data (inc.dst (), FROMSTACK) = inc.src1 () * inc.src2 ();
            break;
        case DIV:
            if (inc.mode () == 0x00)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) / data (inc.src2 (), FROMSTACK);
            else if (inc.mode () == 0x01)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) / inc.src2 ();
            else if (inc.mode () == 0x02)
                data (inc.dst (), FROMSTACK) = inc.src1 () / data (inc.src2 (), FROMSTACK);
            else
                data (inc.dst (), FROMSTACK) = inc.src1 () / inc.src2 ();
            break;
        case MOD:
            if (inc.mode () == 0x00)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) % data (inc.src2 (), FROMSTACK);
            else if (inc.mode () == 0x01)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) % inc.src2 ();
            else if (inc.mode () == 0x02)
                data (inc.dst (), FROMSTACK) = inc.src1 () % data (inc.src2 (), FROMSTACK);
            else
                data (inc.dst (), FROMSTACK) = inc.src1 () % inc.src2 ();
            break;
        case LAND:
            if (inc.mode () == 0x00)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) && data (inc.src2 (), FROMSTACK);
            else if (inc.mode () == 0x01)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) && inc.src2 ();
            else if (inc.mode () == 0x02)
                data (inc.dst (), FROMSTACK) = inc.src1 () && data (inc.src2 (), FROMSTACK);
            else
                data (inc.dst (), FROMSTACK) = inc.src1 () && inc.src2 ();
            break;
        case LOR:
            if (inc.mode () == 0x00)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) || data (inc.src2 (), FROMSTACK);
            else if (inc.mode () == 0x01)
                data (inc.dst (), FROMSTACK) = data (inc.src1 (), FROMSTACK) || inc.src2 ();
            else if (inc.mode () == 0x02)
                data (inc.dst (), FROMSTACK) = inc.src1 () || data (inc.src2 (), FROMSTACK);
            else
                data (inc.dst (), FROMSTACK) = inc.src1 () || inc.src2 ();
            break;
        case LNOT:
            if (inc.mode () == 0x00)
                data (inc.dst (), FROMSTACK) = !data (inc.src1 (), FROMSTACK);
            else if (inc.mode () == 0x01)
                data (inc.dst (), FROMSTACK) = !data (inc.src1 (), FROMSTACK);
            else if (inc.mode () == 0x02)
                data (inc.dst (), FROMSTACK) = !inc.src1 ();
            else
                data (inc.dst (), FROMSTACK) = !inc.src1 ();
            break;
        case CG:
            if (inc.mode () == 0x00)
            {
                if (data (inc.src1 (), FROMSTACK) - data (inc.src2 (), FROMSTACK) > 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            else if (inc.mode () == 0x01)
            {
                if (data (inc.src1 (), FROMSTACK) - inc.src2 () > 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            else if (inc.mode () == 0x02)
            {
                if (inc.src1 () - data (inc.src2 (), FROMSTACK) > 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            else
            {
                if (inc.src1 () - inc.src2 () > 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            break;
        case CL:
            if (inc.mode () == 0x00)
            {
                if (data (inc.src1 (), FROMSTACK) >= data (inc.src2 (), FROMSTACK))
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            else if (inc.mode () == 0x01)
            {
                if (data (inc.src1 (), FROMSTACK) - inc.src2 () >= 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            else if (inc.mode () == 0x02)
            {
                if (inc.src1 () - data (inc.src2 (), FROMSTACK) >= 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            else
            {
                if (inc.src1 () - inc.src2 () >= 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            break;
        case CGE:
            if (inc.mode () == 0x00)
            {
                if (data (inc.src1 (), FROMSTACK) - data (inc.src2 (), FROMSTACK) >= 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            else if (inc.mode () == 0x01)
            {
                if (data (inc.src1 (), FROMSTACK) - inc.src2 () >= 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            else if (inc.mode () == 0x02)
            {
                if (inc.src1 () - data (inc.src2 (), FROMSTACK) >= 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            else
            {
                if (inc.src1 () - inc.src2 () >= 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            break;
        case CLE:
            if (inc.mode () == 0x00)
            {
                if (data (inc.src1 (), FROMSTACK) - data (inc.src2 (), FROMSTACK) > 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            else if (inc.mode () == 0x01)
            {
                if (data (inc.src1 (), FROMSTACK) - inc.src2 () > 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            else if (inc.mode () == 0x02)
            {
                if (inc.src1 () - data (inc.src2 (), FROMSTACK) > 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            else
            {
                if (inc.src1 () - inc.src2 () > 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            break;
        case CE:
            if (inc.mode () == 0x00)
            {
                // cout << "debug src1 data: " << data(inc.src1 (), FROMSTACK) << endl;
                // cout << "debug src1: " << inc.src1 () << endl;
                // cout << "debug src2 data: " << data(inc.src2 (), FROMSTACK) << endl;
                // cout << "debug src2: " << inc.src2 () << endl;
                // cout << "debug dst: " << inc.dst () << endl;
                if (data (inc.src1 (), FROMSTACK) - data (inc.src2 (), FROMSTACK) == 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            else if (inc.mode () == 0x01)
            {
                if (data (inc.src1 (), FROMSTACK) - inc.src2 () == 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            else if (inc.mode () == 0x02)
            {
                if (inc.src1 () - data (inc.src2 (), FROMSTACK) == 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            else
            {
                if (inc.src1 () - inc.src2 () == 0)
                    data (inc.dst (), FROMSTACK) = 1;
                else data (inc.dst (), FROMSTACK) = 0;
            }
            break;
        case CNE:
            if (inc.mode () == 0x00)
            {
                if (data (inc.src1 (), FROMSTACK) - data (inc.src2 (), FROMSTACK) == 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            else if (inc.mode () == 0x01)
            {
                if (data (inc.src1 (), FROMSTACK) - inc.src2 () == 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            else if (inc.mode () == 0x02)
            {
                if (inc.src1 () - data (inc.src2 (), FROMSTACK) == 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            else
            {
                if (inc.src1 () - inc.src2 () == 0)
                    data (inc.dst (), FROMSTACK) = 0;
                else data (inc.dst (), FROMSTACK) = 1;
            }
            break;
        default:
            cout << "hhh" << endl;
            break;
        }
        nextPC ();
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
    else {
        return __stack[addr];
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
    int k = 0;
    std::string addr;
    std::string cs;
    std::string ds;

    std::getline (file, addr);
    std::getline (file, cs);
    std::getline (file, ds);

    uint64_t main_addr = atoull (addr.c_str ());
    uint64_t code_size = atoull (cs.c_str());
    uint64_t data_size = atoull (ds.c_str());

    VM * vm = new VM(main_addr, code_size, data_size);
    while (!file.eof ())
    {
        std::string code;

        std::getline (file, code);
        vm->__code[k] = atoull (code.c_str ());
        k++;
    }
    return vm;
}

uint64_t atoull(const char * str)
{
    stringstream ss;
    uint64_t t;
    ss << str;
    ss >> t;
    return t;
}