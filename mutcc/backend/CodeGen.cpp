//
// Created by guoqi on 10/10/15.
//

#include <string>
#include "CodeGen.h"
#include "frontend/AST.h"

#define BP 0xffffff
#define SP 0xfffffe

using namespace std;

Code CodeGen::generate ()
{
    genOutBlockStmt (__parser.ast ().program ().get ());
    __code.dataSize (__datamap.address ());
    return __code;
}

void CodeGen::genOutBlockStmt (Stmt *stmt)
{
    while (stmt != nullptr)
    {
        switch (stmt->type ()) {
            case NodeType::FuncStmt:
                genFuncStmt (static_cast <FuncStmt *> (stmt));
                break;
            case NodeType::LetStmt:
                genLetStmtOutBlock (static_cast <LetStmt *> (stmt));
                break;
            default:
                break;
        }
        stmt = stmt->next ().get ();
    }
}

uint64_t CodeGen::genInBlockStmt (Stmt *stmt)
{
    uint64_t start = __codemap.curAddr ();
    while (stmt != nullptr)
    {
        switch (stmt->type ()) {
            case NodeType::LetStmt:
                genLetStmtInBlock (static_cast <LetStmt *> (stmt));
                break;
            case NodeType::IfStmt:
                genIfStmt (static_cast <IfStmt *> (stmt));
                break;
            case NodeType::WhileStmt:
                genWhileStmt (static_cast <WhileStmt *> (stmt));
                break;
            case NodeType::ForStmt:
                genForStmt (static_cast <ForStmt *> (stmt));
                break;
            case NodeType::AssignmentStmt:
                genAssignmentStmt (static_cast <AssignmentStmt *> (stmt));
                break;
            case NodeType::ReturnStmt:
                genReturnStmt (static_cast <ReturnStmt *> (stmt));
                break;
            default:
                break;
        }
        stmt = stmt->next ().get ();
    }
    return start;
}

uint64_t CodeGen::genFuncStmt (FuncStmt *stmt)
{
    uint64_t start = __codemap.curAddr ();
    __stackmap.push ();
    // 参数入栈
    for (auto & i : static_pointer_cast <FuncEntry> (stmt->sym_entry)->memlist.table ())
    {
        uint64_t address = __stackmap.mmap (i->typeInfo());
        i->address(address);
    }
    genInBlockStmt (stmt->block_stmt.get ());
    genPopSP ();
    genRet ();
    __stackmap.pop ();
    return start;
}

void CodeGen::genLetStmtOutBlock (LetStmt *stmt)
{
    uint64_t address = __datamap.mmap (static_pointer_cast <VarEntry> (stmt->sym_entry)->typeInfo ());
    static_pointer_cast <VarEntry> (stmt->sym_entry)->address (address);
}

uint64_t CodeGen::genLetStmtInBlock (LetStmt *stmt)
{
    uint64_t address = __stackmap.mmap (static_pointer_cast <VarEntry> (stmt->sym_entry)->typeInfo ());
    static_pointer_cast <VarEntry> (stmt->sym_entry)->address (address);
}

uint64_t CodeGen::genIfStmt (IfStmt *stmt)
{
    uint64_t start = __codemap.curAddr ();

    if (stmt->condition != nullptr) {
        genExp (stmt->condition.get ());
    }

    __stackmap.push ();
    // 预留一条指令，以便后面插入条件跳转指令
    uint64_t jmpaddr = __codemap.mmap ();
    genInBlockStmt (stmt->block_stmt.get ());
    // 向前面预留的位置插入条件跳转指令（为假时跳到当前指令的下一条指令）
    __code.insertInstruction (jmpaddr, __factory.createInstruction (0x45, __codemap.curAddr ()));
    __stackmap.pop ();

    if (stmt->clause_next != nullptr) {
        genIfStmt (static_cast<IfStmt *>(stmt->clause_next.get ()));
    }

    return start;
}

uint64_t CodeGen::genWhileStmt (WhileStmt *stmt)
{
    uint64_t start = __codemap.curAddr ();
    genExp (stmt->condition.get ());    // 先执行一次条件
    uint64_t jmpaddr = __codemap.mmap ();   // **预留一条指令**，以便插入跳转指令
    genInBlockStmt (stmt->block_stmt.get ());    // 循环体
    __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (0x43, start));    // 跳到开头

    // 向前面插入条件跳转指令
    Instruction inc = __factory.createInstruction (0x45, __codemap.curAddr ()); // 生成跳转指令
    __code.insertInstruction (jmpaddr, inc);
    return start;
}

uint64_t CodeGen::genForStmt (ForStmt *stmt)
{
    uint64_t start = __codemap.curAddr ();
    genAssignmentStmt (static_cast <AssignmentStmt *>(stmt->condition.get ())); // 初始化赋值

    uint64_t loopstart = __codemap.curAddr ();
    genExp (stmt->condition.get ());
    uint64_t jmpaddr = __codemap.mmap (); // 预留一条指令，以便后面插入跳转指令
    genInBlockStmt (stmt->block_stmt.get ());
    genAssignmentStmt (static_cast <AssignmentStmt *>(stmt->modify_stmt.get ())); // 每次循环修改变量
    __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (0x43, loopstart));

    // 向前面留下的空位插入条件跳转指令 （为假时直接跳转到当前指令的下一条指令）
    __code.insertInstruction (jmpaddr, __factory.createInstruction (0x45, __codemap.curAddr ()));
    return start;
}

uint64_t CodeGen::genAssignmentStmt (AssignmentStmt *stmt)
{
    uint64_t start = __codemap.curAddr ();

    uint64_t rvalue_tmp = genExp (stmt->rvalue.get ());
    uint64_t size = sizeOf (static_pointer_cast <VarEntry> (stmt->sym_entry)->typeInfo ());
    Instruction inc(0);

    string text = stmt->op->text ();
    if (text == "=") {
        inc = __factory.createInstruction (0x80, size, stmt->sym_entry->address (), rvalue_tmp);
    }
    else if (text == "+=") {
        inc = __factory.createInstruction (0x83, 0x00, stmt->sym_entry->address (), rvalue_tmp);
    }
    else if (text == "-=") {
        inc = __factory.createInstruction (0x84, 0x00, stmt->sym_entry->address (), rvalue_tmp);
    }
    else if (text == "*=") {
        inc = __factory.createInstruction (0x85, 0x00, stmt->sym_entry->address (), rvalue_tmp);
    }
    else if (text == "/=") {
        inc = __factory.createInstruction (0x86, 0x00, stmt->sym_entry->address (), rvalue_tmp);
    }
    else if (text == "%=") {
        inc = __factory.createInstruction (0x87, 0x00, stmt->sym_entry->address (), rvalue_tmp);
    }
    __code.insertInstruction (__codemap.mmap (), inc);

    return start;
}

uint64_t CodeGen::genReturnStmt (ReturnStmt *stmt)
{

}

uint64_t CodeGen::genExp (Exp *exp)
{

}

uint64_t CodeGen::genUnaryExp (UnaryExp *exp)
{

}

uint64_t CodeGen::genBinaryExp (BinaryExp *exp)
{

}

uint64_t CodeGen::genArrayExp (ArrayExp *exp)
{

}

uint64_t CodeGen::genFuncExp (FuncExp *exp)
{

}

uint64_t CodeGen::genAtomicExp (AtomicExp *exp)
{

}


void CodeGen::genPushSP ()
{
    Instruction inc = __factory.createInstruction (0x80, 0x00, BP, SP);
    __code.insertInstruction (__codemap.mmap (), inc);
}

void CodeGen::genPopSP ()
{
    Instruction inc = __factory.createInstruction (0x80, 0x00, SP, BP);
    __code.insertInstruction (__codemap.mmap (), inc);
}

void CodeGen::genRet ()
{
    Instruction inc = __factory.createInstruction (0x01);
    __code.insertInstruction (__codemap.mmap (), inc);
}

void CodeGen::genHalt ()
{
    Instruction inc = __factory.createInstruction (0x00);
    __code.insertInstruction (__codemap.mmap (), inc);
}
