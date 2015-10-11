//
// Created by guoqi on 10/10/15.
//

#ifndef MUTCC_CODEGEN_H
#define MUTCC_CODEGEN_H

#include "MemoryMap.h"
#include "Code.h"
#include "frontend/Parser.h"
#include "frontend/AST.h"

class CodeGen
{
public:
    CodeGen(MutCParser & parser): __parser(parser) {}

    Code generate();

private:
    void genOutBlockStmt (Stmt * stmt);
    void genLetStmtOutBlock (LetStmt * stmt);

    // 以下函数均返回所生成的指令串的**起始地址**
    uint64_t genInBlockStmt (Stmt *stmt);
    uint64_t genFuncStmt (FuncStmt *stmt);
    uint64_t genLetStmtInBlock (LetStmt *stmt);
    uint64_t genIfStmt (IfStmt *stmt);
    uint64_t genWhileStmt (WhileStmt *stmt);
    uint64_t genForStmt (ForStmt *stmt);
    uint64_t genAssignmentStmt (AssignmentStmt *stmt);
    uint64_t genReturnStmt (ReturnStmt *stmt);

    // 以下函数均返回存储表达式结果的**临时变量的地址**
    uint64_t genExp (Exp *exp);
    uint64_t genUnaryExp (UnaryExp *exp);
    uint64_t genBinaryExp (BinaryExp *exp);
    uint64_t genArrayExp (ArrayExp *exp);
    uint64_t genFuncExp (FuncExp *exp);
    uint64_t genAtomicExp (AtomicExp *exp);

    // 一些固定指令
    void genPushSP();
    void genPopSP();
    void genRet();
    void genHalt();

private:
    MutCParser  &       __parser;
    Code                __code;
    InstructionFactory  __factory;

    CodeMemoryMap       __codemap;
    DataMemoryMap       __datamap;
    StackMemoryMap      __stackmap;
};

#endif //MUTCC_CODEGEN_H
