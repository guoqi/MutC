//
// Created by guoqi on 10/27/15.
//

#ifndef MUTCC_MISCELL_H
#define MUTCC_MISCELL_H

#include <map>
#include <string>
#include "Instruction.h"

using namespace std;

const map<string, uint64_t> BinaryOp2Inc = {
    {"+", ADD},
    {"-", SUB},
    {"*", MUL},
    {"/", DIV},
    {"%", MOD},
    {"and", LAND},
    {"or", LOR},
    {"==", CE},
    {"!=", CNE},
    {">", CG},
    {"<", CL},
    {">=", CGE},
    {"<=", CLE},
};

#endif //MUTCC_MISCELL_H
