//
// Created by guoqi on 11/2/15.
//

#ifndef MUTCVM_VIRTUALMACHINE_H
#define MUTCVM_VIRTUALMACHINE_H

#include <cstdint>
#include <fstream>
#include <sstream>
#include "backend/Size.h"
#include "backend/Instruction.h"
#include "backend/Code.h"

#define FROMDATA    1
#define FROMSTACK   2

using namespace std;

class VM {
public:
    VM (uint64_t main_addr, uint64_t code_size, uint64_t data_size);
    ~VM ();

    void execute();

    friend VM * initVM(ifstream & file);

private:
    // 取指令，默认会修改pc的值
    Instruction fetchInc();

    // 取数据，flag为0表示从静态存储区取，为1表示从栈中取
    uint64_t & data(uint64_t addr, int flag);

    void jmpTo(uint64_t addr);
    void nextPC();

private:
    uint64_t        __pc;
    uint64_t        __bp;
    uint64_t        __sp;

    Instruction *   __code;
    uint64_t  *     __data;
    uint64_t  *     __stack;
};

VM * initVM(ifstream & file);
uint64_t atoull(const char * str);

#endif //MUTCVM_VIRTUALMACHINE_H
