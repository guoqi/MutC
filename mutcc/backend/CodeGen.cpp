//
// Created by guoqi on 10/10/15.
//

#include <string>
#include "CodeGen.h"
#include "Instruction.h"
#include "Miscell.h"
#include "frontend/AST.h"


using namespace std;

Code & CodeGen::generate ()
{
    genOutBlockStmt (__parser->ast ().program ().get ());
    __code.dataSize (__datamap.address ());
    if (__code.mainAddr () == -1) {
        // TODO error handling: the program must have main function at least.
    }
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
    for (auto & i : static_cast <FuncEntry *> (stmt->sym_entry)->memlist.table ())
    {
        uint64_t address = __stackmap.mmap (i->typeInfo());
        i->address(address);
    }
    genInBlockStmt (stmt->block_stmt.get ());
    genPopSP ();
    if (stmt->name->text () == "main") {
        __code.mainAddr (start);
        genHalt ();
    }
    else {
        genRet ();
    }
    __stackmap.pop ();
    return start;
}

void CodeGen::genLetStmtOutBlock (LetStmt *stmt)
{
    uint64_t address = __datamap.mmap (static_cast <VarEntry *> (stmt->sym_entry)->typeInfo ());
    static_cast <VarEntry *> (stmt->sym_entry)->address (address);
}

uint64_t CodeGen::genLetStmtInBlock (LetStmt *stmt)
{
    uint64_t address = __stackmap.mmap (static_cast <VarEntry *> (stmt->sym_entry)->typeInfo ());
    static_cast <VarEntry *> (stmt->sym_entry)->address (address);
}

uint64_t CodeGen::genIfStmt (IfStmt *stmt)
{
    uint64_t start = __codemap.curAddr ();
    uint64_t rslt_addr = genExp (stmt->condition.get ());

    __stackmap.push ();
    // **预留一条指令**，以便后面插入条件跳转指令
    uint64_t jmpaddr = __codemap.mmap ();
    genInBlockStmt (stmt->block_stmt.get ());
    // 向前面预留的位置插入条件跳转指令（为假时跳到当前指令的下一条指令）
    __code.insertInstruction (jmpaddr, __factory.createInstruction (JZ, AD, 0x08, rslt_addr, __codemap.curAddr ()));
    __stackmap.pop ();

    if (stmt->clause_next != nullptr) {
        genIfStmt (static_cast<IfStmt *>(stmt->clause_next.get ()));
    }

    return start;
}

uint64_t CodeGen::genWhileStmt (WhileStmt *stmt)
{
    uint64_t start = __codemap.curAddr ();
    uint64_t rslt_addr = genExp (stmt->condition.get ());    // 先执行一次条件
    uint64_t jmpaddr = __codemap.mmap ();   // **预留一条指令**，以便插入跳转指令
    genInBlockStmt (stmt->block_stmt.get ());    // 循环体
    __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (JMP, AD, 0x00, NIL, start));    // 跳到开头

    // 向前面插入条件跳转指令
    Instruction inc = __factory.createInstruction (JZ, AD, 0x08, rslt_addr, __codemap.curAddr ()); // 生成跳转指令
    __code.insertInstruction (jmpaddr, inc);
    return start;
}

uint64_t CodeGen::genForStmt (ForStmt *stmt)
{
    uint64_t start = __codemap.curAddr ();
    genAssignmentStmt (static_cast <AssignmentStmt *>(stmt->initial_stmt.get ())); // 初始化赋值

    uint64_t loopstart = __codemap.curAddr ();
    uint64_t rslt_addr = genExp (stmt->condition.get ());
    uint64_t jmpaddr = __codemap.mmap (); // 预留一条指令，以便后面插入跳转指令
    genInBlockStmt (stmt->block_stmt.get ());
    genAssignmentStmt (static_cast <AssignmentStmt *>(stmt->modify_stmt.get ())); // 每次循环修改变量
    __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (JMP, AD, 0x00, NIL, loopstart));

    // 向前面留下的空位插入条件跳转指令 （为假时直接跳转到当前指令的下一条指令）
    __code.insertInstruction (jmpaddr, __factory.createInstruction (JZ, AD, 0x08, rslt_addr, __codemap.curAddr ()));
    return start;
}

uint64_t CodeGen::genAssignmentStmt (AssignmentStmt *stmt)
{
    uint64_t start = __codemap.curAddr ();

    uint64_t rvalue_tmp = genExp (stmt->rvalue.get ());
    uint64_t size = sizeOf (static_cast <VarEntry *> (stmt->sym_entry)->typeInfo ());
    Instruction inc(0);

    string text = stmt->op->text ();
    if (text == "=") {
        inc = __factory.createInstruction (MOV, MM, size, stmt->sym_entry->address (), rvalue_tmp, NIL);
    }
    else if (text == "+=") {
        inc = __factory.createInstruction (ADD, MM, size, stmt->sym_entry->address (), rvalue_tmp, stmt->sym_entry->address ());
    }
    else if (text == "-=") {
        inc = __factory.createInstruction (SUB, MM, size, stmt->sym_entry->address (), rvalue_tmp, stmt->sym_entry->address ());
    }
    else if (text == "*=") {
        inc = __factory.createInstruction (MUL, MM, size, stmt->sym_entry->address (), rvalue_tmp, stmt->sym_entry->address ());
    }
    else if (text == "/=") {
        inc = __factory.createInstruction (DIV, MM, size, stmt->sym_entry->address (), rvalue_tmp, stmt->sym_entry->address ());
    }
    else if (text == "%=") {
        inc = __factory.createInstruction (MOD, MM, size, stmt->sym_entry->address (), rvalue_tmp, stmt->sym_entry->address ());
    }
    __code.insertInstruction (__codemap.mmap (), inc);

    return start;
}

uint64_t CodeGen::genReturnStmt (ReturnStmt *stmt)
{
    uint64_t start = __codemap.curAddr ();

    uint64_t rslt_addr = genExp(stmt->rslt.get ());
    uint64_t size = sizeOf (stmt->rslt->typeInfo ());   // TODO 注意表达式的类型推断部分还没完成，而该处需要依赖类型推断，或者依赖函数本身的信息，此处待定

    // 将结果放入函数栈开头，由函数调用部分负责取出
    __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (MOV, MM, size, __stackmap.bp (), rslt_addr));

    return start;
}

uint64_t CodeGen::genExp (Exp *exp)
{
    switch (exp->type ()) {
        case NodeType::UnaryExp:
            return genUnaryExp (static_cast<UnaryExp *>(exp));
        case NodeType::BinaryExp:
            return genBinaryExp (static_cast <BinaryExp *> (exp));
        case NodeType::ArrayExp:
            return genArrayExp (static_cast<ArrayExp *>(exp));
        case NodeType::FuncExp:
            return genFuncExp (static_cast<FuncExp *>(exp));
        case NodeType::AtomicExp:
            return genAtomicExp (static_cast<AtomicExp *>(exp));
        default:
            break;
    }
    return 0;
}

uint64_t CodeGen::genUnaryExp (UnaryExp *exp)
{
    string text = exp->op ()->text ();

    uint64_t value_addr = genExp (exp->expr1.get ());
    uint64_t size = sizeOf (exp->expr1->typeInfo ());
    Instruction inc(0);
    uint64_t addr;

    if (text == "+") {
        addr = __stackmap.mmap (size);
        inc = __factory.createInstruction (MOV, MM, size, addr, value_addr, NIL);
    }
    else if (text == "-") {
        addr = __stackmap.mmap (size);
        inc = __factory.createInstruction (SUB, IM, size, addr, 0, value_addr);
    }
    else if (text == "@") {
        addr = __stackmap.mmap (0x08);
        inc = __factory.createInstruction (MOV, IM, 0x08, addr, value_addr, NIL);
    }
    else if (text == "*") {
        uint64_t tmp_addr = __stackmap.mmap (0x08);
        __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (MOV, MM, 0x08, tmp_addr, value_addr, NIL));
        size = sizeOf (exp->expr1->typeInfo ()->next ());
        addr = __stackmap.mmap (size);
        inc = __factory.createInstruction (MOV, MM, size, addr, tmp_addr, NIL);
    }
    else if (text == "not") {
        addr = __stackmap.mmap (size);
        inc = __factory.createInstruction (LNOT, MM, size, addr, value_addr, NIL);
    }
    else {
        // TODO error handling
    }

    __code.insertInstruction (__codemap.mmap (), inc);

    return addr;
}

uint64_t CodeGen::genBinaryExp (BinaryExp *exp)
{
    string text = exp->op ()->text ();

    uint64_t left_addr = genExp (exp->expr1.get ());
    uint64_t right_addr = genExp (exp->expr2.get ());
    uint64_t size = sizeOf (exp->expr1->typeInfo ());   // 以左子树为准
    uint64_t addr;
    Instruction inc(0);

    map<string, uint64_t>::const_iterator iter = BinaryOp2Inc.find (text);
    if (iter != BinaryOp2Inc.end ()) {
        addr = __stackmap.mmap (size);
        inc = __factory.createInstruction (iter->second, MM, size, addr, left_addr, right_addr);
    }
    else if (text == ".") {
        // TODO
    }
    else if (text == "->") {
        // TODO
    }
    else {
        // TODO error handling
    }

    __code.insertInstruction (__codemap.mmap (), inc);

    return addr;
}

uint64_t CodeGen::genArrayExp (ArrayExp *exp)
{
    uint64_t addr;

    uint64_t size = sizeOf (exp->typeInfo ());
    addr = __stackmap.mmap (size);

    // 求下标表达式的值
    vector<uint64_t> index_addr;
    for (auto & iter : exp->index_list) {
        index_addr.push_back (genExp (iter.get ()));
    }

    // TODO

    uint64_t right_addr = exp->sym_entry->address ();
    //__code.insertInstruction (__codemap.mmap (), __factory.createInstruction (MOV, MM, size, addr, ,NIL));

    return addr;
}

uint64_t CodeGen::genFuncExp (FuncExp *exp)
{
    uint64_t addr;

    vector<uint64_t> param_addr;

    for (auto & iter : exp->param_list) {
        param_addr.push_back (genExp (iter.get ()));
    }

    __stackmap.push ();
    vector<Exp::Ptr>::iterator param_list_iter = exp->param_list.begin ();
    for (auto & iter : param_addr) {
        uint64_t size = sizeOf (param_list_iter->get ()->typeInfo ());
        __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (MOV, MM, size, __stackmap.mmap (size), iter, NIL));
        ++param_list_iter;
    }
    __stackmap.pop ();

    genPushSP ();
}

uint64_t CodeGen::genAtomicExp (AtomicExp *exp)
{
    uint64_t addr;

    if (exp->var->type () == TokenType::Identifier) {
        return exp->sym_entry->address ();
    }
    else {
        addr = __stackmap.mmap (sizeOf (exp->typeInfo ()));
        if (exp->var->type () == TokenType::Integer) {
            __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (MOV, IM, sizeOf (exp->typeInfo ()), addr, exp->var->value ().integer, NIL));
        }
        else {
            __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (MOV, IM, sizeOf (exp->typeInfo ()), addr, (uint64_t)exp->var->value ().real, NIL));
        }
    }
    return addr;
}


void CodeGen::genPushSP ()
{
    __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (PUSHSP));
}

void CodeGen::genPopSP ()
{
    __code.insertInstruction (__codemap.mmap (), __factory.createInstruction (POPSP));
}

void CodeGen::genRet ()
{
    Instruction inc = __factory.createInstruction (RET);
    __code.insertInstruction (__codemap.mmap (), inc);
}

void CodeGen::genHalt ()
{
    Instruction inc = __factory.createInstruction (HALT);
    __code.insertInstruction (__codemap.mmap (), inc);
}
